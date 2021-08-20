from os import path
import json
import subprocess
import os
import geopy.distance
import statistics
import shutil
import copy
# Get Directory
BASE_DIR = path.dirname(path.dirname(path.abspath(__file__)))

directory = path.join(BASE_DIR, 'data/model_apply_outputs/Instances')

if not os.path.exists(directory):
    os.makedirs(directory)
    
#####################
#1 Read Input Data
#####################

# Reading route data
print('Reading Route Data')
route_data_path=path.join(BASE_DIR, 'data/model_apply_inputs/new_route_data.json')
with open(route_data_path, newline='') as in_file:
    actual_routes = json.load(in_file)
  
# Read travel time data
print('Reading Travel Time Data')
travel_times_path=path.join(BASE_DIR, 'data/model_apply_inputs/new_travel_times.json')
with open(travel_times_path, newline='') as in_file:
    travel_times = json.load(in_file)

# Read package data
print('Reading Package Data')
package_data_path=path.join(BASE_DIR, 'data/model_apply_inputs/new_package_data.json')
with open(package_data_path, newline='') as in_file:
    package_data = json.load(in_file)


# Route IDs
route_ids = actual_routes.keys()

print('Reading High Score Route IDs')
route_id_vector = []
for key in route_ids:
    route_id_vector.append(key)

#route_id_vector = route_id_vector[0:500]
print ('---------------')   


#####################
#2 Write Instances
#####################
if(True):
    print('Reading TWs, Service Time and Departure Time')
    TWs_per_stops_per_route = []
    nb_TWs_of_routes = []
    service_times_per_stop_per_route = []
    planned_service_time_of_routes = []
    departure_times_of_routes_in_seconds = []
    cost_mat_kmh_of_routes = []
    count = -1
    for route_id in route_id_vector:
        count+=1
        print(count)
        # Departure times
        time_in_seconds = (float(actual_routes[route_id]['departure_time_utc'][:2])*3600.0 + 
        float(actual_routes[route_id]['departure_time_utc'][3:5])*60.0+
        float(actual_routes[route_id]['departure_time_utc'][6:8]))
        
        departure_times_of_routes_in_seconds.append(time_in_seconds)
        
        # Service Times and TWs
        planned_service_time_per_route = 0
        service_times_per_stop = []
        nb_TWs_per_route = 0
        TWs_per_stops = []
        stop_ids = actual_routes[route_id]['stops'].keys()
        stop_ids_vector = []
        
        
        for stop_id in stop_ids:
            if actual_routes[route_id]['stops'][stop_id]['type']!="Station":
                service_time_sum = 0;
                parcel_ids=package_data[route_id][stop_id].keys()
                min_tw=time_in_seconds
                max_tw= time_in_seconds+ 24.0*3600.0
                for parcel_id in parcel_ids:
                    service_time_sum+=float(package_data[route_id][stop_id][parcel_id]['planned_service_time_seconds'])
                    
                    if str(package_data[route_id][stop_id][parcel_id]['time_window']['start_time_utc'])!='nan':
                        nb_TWs_per_route+=1
                        
                        min_tw_seconds = (float(package_data[route_id][stop_id][parcel_id]['time_window']['start_time_utc'][11:13])*3600
                        +float(package_data[route_id][stop_id][parcel_id]['time_window']['start_time_utc'][14:16])*60)
                        
                        max_tw_seconds = (float(package_data[route_id][stop_id][parcel_id]['time_window']['end_time_utc'][11:13])*3600
                        +float(package_data[route_id][stop_id][parcel_id]['time_window']['end_time_utc'][14:16])*60)
                        
                        min_tw = round(max([min_tw_seconds, min_tw]),2)
                        max_tw = round(min([max_tw_seconds, max_tw]),2)
                    if max_tw==0.0:
                        max_tw = time_in_seconds + 24.0*3600.0
                    if min_tw>max_tw:
                        max_tw = time_in_seconds + 24.0*3600.0
                    
                TWs_per_stops.append([min_tw, max_tw])
                stop_ids_vector.append(stop_id)
                service_times_per_stop.append(round(service_time_sum,2))
                planned_service_time_per_route+=service_time_sum
        
        service_times_per_stop_per_route.append({stop_ids_vector[i]:service_times_per_stop[i] for i in range(0, len(stop_ids_vector))})
        planned_service_time_of_routes.append(round(planned_service_time_per_route,2))
        TWs_per_stops_per_route.append({stop_ids_vector[i]:TWs_per_stops[i] for i in range(0, len(stop_ids_vector))})
        nb_TWs_of_routes.append(nb_TWs_per_route)
        
        
        # Read Cost Matrix
        nodes = []
        for stop_id in stop_ids:
            nodes.append(stop_id)
        
        for node in nodes:
            if actual_routes[route_id]['stops'][node]['type']=="Station":
                DepotNode = node
            
        nodes.remove(DepotNode)
        
        CostMat_actual =[]
        Sum = 0
        if(True):
            Cost=[0.0]
            for node1 in nodes:
                Cost.append(travel_times[route_id][DepotNode][node1])
            CostMat_actual.append(Cost)
            Sum+=sum(Cost)
            
            for node in nodes:
                Cost=[travel_times[route_id][node][DepotNode]]
                for node1 in nodes:
                    Cost.append(travel_times[route_id][node][node1])
                CostMat_actual.append(Cost)
                Sum+=sum(Cost)
            
            nb_nodes = len(CostMat_actual) 
        
        # Calculate Geodistance CostMatrix
        if(True):
            CostMat_geodist =[]
            Sum_geodist = 0
            Cost=[0.0]
            for node1 in nodes:
                #coords(lat, lon)
                coords_1 = (actual_routes[route_id]['stops'][DepotNode]['lat'], actual_routes[route_id]['stops'][DepotNode]['lng'])
                coords_2 = (actual_routes[route_id]['stops'][node1]['lat'], actual_routes[route_id]['stops'][node1]['lng'])
                Cost.append(round(geopy.distance.great_circle(coords_1, coords_2).km,4))
                 
            CostMat_geodist.append(Cost)
            Sum_geodist+=sum(Cost)

            
            for node in nodes:           
                coords_1 = (actual_routes[route_id]['stops'][node]['lat'], actual_routes[route_id]['stops'][node]['lng'])
                coords_2 = (actual_routes[route_id]['stops'][DepotNode]['lat'], actual_routes[route_id]['stops'][DepotNode]['lng'])
                Cost=[round(geopy.distance.great_circle(coords_1, coords_2).km,4)]
                
                
                for node1 in nodes:
                    coords_2 = (actual_routes[route_id]['stops'][node1]['lat'], actual_routes[route_id]['stops'][node1]['lng'])
                    Cost.append(round(geopy.distance.great_circle(coords_1, coords_2).km,4))
                    
                CostMat_geodist.append(Cost)
                Sum_geodist+=sum(Cost)
                 
            cost_mat_kmh_of_routes.append(Sum_geodist/(Sum))
        
        
        #Adapt CostMatrix if given distance seems unlikely high compared to geodistance
        if(True):
            CostMat = copy.copy(CostMat_actual)
            for i in range(1,len(CostMat_actual)):
                time_per_node = sum(CostMat_actual[i])
                km_per_node  = sum(CostMat_geodist[i])
                kmh_per_node = km_per_node/time_per_node

                for j in range(1,len(CostMat_actual)):
                    Cost_reverse = CostMat_actual[j][i]
                    Cost_actual = CostMat_actual[i][j]
                    Cost_geodist = CostMat_geodist[i][j]//kmh_per_node
                    
                    if (Cost_actual>Cost_geodist*1.3):
                        CostMat[i][j] = min(Cost_actual,Cost_reverse) 

                    
        nb_nodes = len(CostMat)
        

        #HALNS Input
        if(True):
            
            vrp_path=path.join(BASE_DIR, 'data/model_apply_outputs/Instances/', route_id)
            
            fileID = open(vrp_path, "w")
            
            fileID.write("Nodes : ")
            fileID.write(str(nb_nodes))
            fileID.write("\n")
            
            fileID.write("Start Time : ")
            fileID.write(str(time_in_seconds))
            fileID.write("\n")
            
            for i in range(0,nb_nodes):
                str1 = ' '.join(str(e) for e in CostMat[i])
                fileID.write(str1)
                fileID.write('\n')
            
            fileID.write("Service Times : \n")
            for i in range(0,nb_nodes-1):
                fileID.write(str(service_times_per_stop[i]))
                fileID.write('\n')
            
            fileID.write("Time Windows : \n")
            for i in range(0,nb_nodes-1):
                fileID.write( (str(TWs_per_stops[i][0]) +" "+ str(TWs_per_stops[i][1])))
                fileID.write('\n')
            
            
            fileID.close()
            
        
        
#####################
#3 Solve Instances with Hybrid Adaptive Large Neighborhood Search, compiled in model_build_outputs
#####################
if(True):
    HALNS_path=path.join(BASE_DIR, 'data/model_build_outputs/')
    
    heuristics_location =path.join(BASE_DIR, 'data/model_apply_outputs/')
    print('Solving Instances with HALNS')
    os.chdir(HALNS_path)
    shutil.copy("HALNS.out", heuristics_location)
       
    os.chdir(heuristics_location)
    #print(os.listdir())
    
    print(heuristics_location)
    count = 0
    child_processes = []
    Parallel = 0   
    
    Vector_parallel =[]  
    for route_id in route_id_vector:    
        count+=1
        print(count)         
        Parallel +=1  
        p = subprocess.Popen(str(heuristics_location+"HALNS.out" + " " + route_id + " 0 1"), stdout=subprocess.PIPE, shell=True)
        #p.communicate()
        child_processes.append(p)    
        if Parallel==16 or count==len(route_id_vector):
            for cp in child_processes:
                cp.communicate()
            Parallel=0
            child_processes=[]
            
    print ('---------------') 

                
#####################
#4 Read Results and Write proposed_sequences
#####################
if(True):    
    print('Reading HALNS Solutions and Write model_apply_ouputs')
    travel_times_withoutTWs_routes=[]
    RouteDicts = []
    count = 0
    for route_id in route_id_vector:
        count+=1
        print(count)
        nodes = []
        for key in travel_times[route_id]:
            nodes.append(key)
        
        for node in nodes:
            if actual_routes[route_id]['stops'][node]['type']=="Station":
                DepotNode = node
        
        
        nodes.remove(DepotNode)    
        
        nodes = [DepotNode] + nodes
        
        file_location = path.join(BASE_DIR, 'data/model_apply_outputs/',str(route_id + "-sol-1.txt"))
                    
        with open(file_location, 'r') as input_data_file:
            input_data = input_data_file.read()
        

        lines = input_data.split('\n')
    
        HALNS_solution = [int(lines[i]) for i in range(4,4+len(nodes))] 
        
        
        solution_sorted_by_index = []
        for i in HALNS_solution:
            solution_sorted_by_index.append(nodes[i])
        
        RouteDict = {solution_sorted_by_index[i]: i  for i in range(0,len(nodes))}
        RouteDict = dict(sorted(RouteDict.items(), key=lambda item: item[1]))
        
        RouteDicts.append(RouteDict)
        
        sequence_route_1 = RouteDict
        travel_times_route_1 = travel_times[route_id]
           
        stops = []
        
        for key in sequence_route_1:
            stops.append(key)
        
        total_travel_times_route_1 = 0
        
        for i in range(0,len(stops)-1):
            stop_i = stops[i]
            stop_j = stops[i+1]        
            total_travel_times_route_1+=travel_times_route_1[stop_i][stop_j]
        
        total_travel_times_route_1+=travel_times_route_1[stop_j][stops[0]]
        
        travel_times_withoutTWs_routes.append(round(total_travel_times_route_1,2))
    
    output={route_id_vector[i]:{'proposed': RouteDicts[i]} for i in range(0,len(route_id_vector))}
    
    output_path=path.join(BASE_DIR, 'data/model_apply_outputs/proposed_sequences.json')
    with open(output_path, 'w') as out_file:
        json.dump(output, out_file)
        print("Success: The '{}' file has been saved".format(output_path))
    
    print('Done!')
    

