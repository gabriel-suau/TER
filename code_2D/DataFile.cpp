/*!
 * @file DataFile.cpp
 *
 * Handles the reading of the data file.
 *
 * @authors Gabriel Suau, Remi Pegouret, Lucas Trautmann
 *
 * @version 0.1.0
 *
 * @copyright © 2021 Gabriel Suau
 * @copyright © 2021 Remi Pegouret
 * @copyright © 2021 Lucas Trautmann
 * 
 * @copyright This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * @copyright This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * @copyright You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "DataFile.h"
#include "termcolor.h"

#include "Eigen/Eigen/Dense"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <regex>

DataFile::DataFile()
{
}

DataFile::DataFile(const std::string& fileName):
  _fileName(fileName), _scenario("none")
{
}

void DataFile::Initialize(const std::string& fileName)
{
  _fileName = fileName;
  _scenario = "none";
}

std::string DataFile::cleanLine(std::string &line)
{
  std::string res = line;

  // Remove everything after a possible #
  res = regex_replace(res, std::regex("#.*$"), std::string(""));
  // Replace tabulation(s) by space(s)
  res = regex_replace(res, std::regex("\t"), std::string(" "), std::regex_constants::match_any);
  // Replace multiple spaces by 1 space
  res = regex_replace(res, std::regex("\\s+"), std::string(" "), std::regex_constants::match_any);
  // Remove any leading spaces
  res = regex_replace(res, std::regex("^ *"), std::string(""));

  return res;
}

void DataFile::readDataFile()
{
  // Open the data file
  std::ifstream data_file(_fileName.data());
  if (!data_file.is_open())
    {
      std::cout << termcolor::red << "ERROR::DATAFILE : Unable to open file " << _fileName << std::endl;
      std::cout << termcolor::reset;
      exit(-1);
    }
  else
    {
      std::cout << "====================================================================================================" << std::endl;
      std::cout << "Reading data file " << _fileName << std::endl;
    }
  // Pour stocker chaque ligne
  std::string line;
  // Run through the data_file to find the parameters
  while (getline(data_file, line))
    {
      // Clean line
      std::string proper_line(cleanLine(line));
      if (proper_line.find("TimeScheme") != std::string::npos)
        {
          data_file >> _timeScheme;
        }
      if (proper_line.find("NumericalFlux") != std::string::npos)
        {
          data_file >> _numericalFlux;
        }
      if (proper_line.find("ResultsDir") != std::string::npos)
        {
          data_file >> _resultsDir;
        }
      if (proper_line.find("MeshFile") != std::string::npos)
        {
          data_file >> _meshFile;
        }
      if (proper_line.find("InitialTime") != std::string::npos)
        {
          data_file >> _initialTime;
        }
      if (proper_line.find("FinalTime") != std::string::npos)
        {
          data_file >> _finalTime;
        }
      if (proper_line.find("TimeStep") != std::string::npos)
        {
          data_file >> _timeStep;
        }
      if (proper_line.find("CFL") != std::string::npos)
        {
          data_file >> _CFL;
        }
      if (proper_line.find("GravityAcceleration") != std::string::npos)
        {
          data_file >> _g;
        }
      if (proper_line.find("SaveFrequency") != std::string::npos)
        {
          data_file >> _saveFrequency;
        }
      if (proper_line.find("Scenario") != std::string::npos)
        {
          data_file >> _scenario;
        }
      if (proper_line.find("IsTopography") != std::string::npos)
        {
          data_file >> _isTopography;
        }
      if (proper_line.find("TopographyType") != std::string::npos)
        {
          data_file >> _topographyType;
        }
      if (proper_line.find("TopographyFile") != std::string::npos)
        {
          data_file >> _topographyFile;
        }
      if (proper_line.find("BoundaryConditions") != std::string::npos)
        {
          data_file >> _nBoundaries;
          _boundaryConditionReference.resize(_nBoundaries);
          _boundaryConditionType.resize(_nBoundaries);
          for (int i(0) ; i<_nBoundaries ; i++)
          {
            data_file >> _boundaryConditionReference(i) >> _boundaryConditionType[i];
          }
        }
    }

  // Création et nettoyage du dossier de résultats
  std::cout << "Creating the results directory..." << std::endl;
  system(("mkdir -p ./" +_resultsDir).c_str());
  system(("rm -f ./" +_resultsDir + "/solution*").c_str());
  system(("cp -r ./" + _fileName + " ./" + _resultsDir + "/params.txt").c_str());

  // Logs
  std::cout << termcolor::green << "SUCCESS::DATAFILE : Results directory created successfully !" << std::endl;
  std::cout << termcolor::reset;

  // Pour le scénario LaSalie, impose la topographie et les CL
  if (_scenario == "LaSalie")
    {
      _isTopography = true;
      _topographyType = "File";
      _topographyFile = "topography_la_salie.csv";
    }

  // Si pas de topo --> impose un fond plat
  if (_isTopography == false)
    {
      _topographyType = "FlatBottom";
    }
  // Logs de succès
  std::cout << termcolor::green << "SUCCESS::DATAFILE : File read successfully" << std::endl;
  std::cout << termcolor::reset << "====================================================================================================" << std::endl << std::endl;
}


// Affiche les paramètres sur le terminal
void DataFile::printData() const
{
  std::cout << "====================================================================================================" << std::endl;
  std::cout << "Printing parameters of " << _fileName << std::endl;
  std::cout << "Mesh                = Get from file" << std::endl;
  std::cout << "Mesh file           = " << _meshFile << std::endl;
  std::cout << "Boundary conditions = " << _nBoundaries << std::endl;
  for (int i(0) ; i < _nBoundaries ; ++i)
    {
      std::cout << "   " << _boundaryConditionReference(i) << " " << _boundaryConditionType[i] << std::endl; 
    }
  std::cout << "Time Scheme         = " << _timeScheme << std::endl;
  std::cout << "Initial time        = " << _initialTime << std::endl;
  std::cout << "Final time          = " << _finalTime << std::endl;
  std::cout << "Time step           = " << _timeStep << std::endl;
  std::cout << "Gravity             = " << _g << std::endl;
  std::cout << "Numerical Flux      = " << _numericalFlux << std::endl;
  std::cout << "Results directory   = " << _resultsDir << std::endl;
  std::cout << "Save Frequency      = " << _saveFrequency << std::endl;
  std::cout << "Scenario            = " << _scenario << std::endl;
  std::cout << "Topography          = " << _topographyType << std::endl;
  if (_topographyType == "File")
    {
      std::cout << "Topography file     = " << _topographyFile << std::endl;
    }
  std::cout << "====================================================================================================" << std::endl << std::endl;
}
