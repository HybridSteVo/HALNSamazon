import os
from os import path
import subprocess
import shutil

# Get Directory
BASE_DIR = path.dirname(path.dirname(path.abspath(__file__)))

#####################
#1 Compile C++
#####################

# Read input data
print('Compiling C++')

CCC_DIR=os.path.join(BASE_DIR, 'src/HALNSamazon')
os.chdir(CCC_DIR)
     
p = subprocess.Popen("g++ -o HALNS.out -O3 -march=native -frename-registers HALNSamazon.cpp ALNSFunctions.cpp AuxiliaryFunctions.cpp CostCalculations.cpp GenerateStartSolutions.cpp InsertOperators.cpp ReadOriginalInstance.cpp RemoveOperators.cpp SimulatedAnnealing.cpp TimeWindowFunctions.cpp WriteSolutionFile.cpp",shell=True, stdout=subprocess.PIPE)
p.communicate()

print ('---------------') 

#####################
#2 Copy to model_build_outputs
#####################

model_path=path.join(BASE_DIR, 'data/model_build_outputs/')
shutil.copy("HALNS.out", model_path)

# Write output data

print("Success: HALNS has been saved to {}".format(model_path))
