#ifndef TIME_SCHEME_H
#define TIME_SCHEME_H

#include "Eigen/Eigen/Dense"
#include "Eigen/Eigen/Sparse"
#include "DataFile.h"
#include "Mesh.h"
#include "Physics.h"
#include "FiniteVolume.h"

#include <vector>



class TimeScheme
{
protected:
  // Pointeur vers les trucs importants
  DataFile* _DF;
  Mesh* _mesh;
  Physics* _physics;
  FiniteVolume* _finVol;

  // Vecteur solution
  Eigen::Matrix<double, Eigen::Dynamic, 2> _Sol;

  // Paramètres de temps
  double _timeStep;
  double _initialTime;
  double _finalTime;
  double _currentTime;

  // Probes
  int _nProbes;
  std::vector<int> _probesRef;
  std::vector<double> _probesPos;
  std::vector<int> _probesIndices;
  
public:
  // Constructeurs
  TimeScheme();
  TimeScheme(DataFile* DF, Mesh* mesh, Physics* physics, FiniteVolume* finVol);

  // Initialiseur
  void Initialize(DataFile* DF, Mesh* mesh, Physics* physics, FiniteVolume* finVol);
  // Destructeur
  virtual ~TimeScheme() = default;

  // Getters
  const Eigen::Matrix<double, Eigen::Dynamic, 2>& getSolution() const {return _Sol;};
  double getTimeStep() const {return _timeStep;};
  double getInitialTime() const {return _initialTime;};
  double getFinalTime() const {return _finalTime;};
  double getCurrentTime() const {return _currentTime;};

  // Adjust the probes prositions to fit within the mesh
  void buildProbesCellIndices();
  
  // Solve and save solution
  virtual void oneStep() = 0;
  void saveCurrentSolution(std::string& fileName) const;
  void saveProbes() const;
  void solve();

  // Error
  Eigen::Vector2d computeL2Error() const;
  Eigen::Vector2d computeL1Error() const;
};



class ExplicitEuler: public TimeScheme
{
public:
  // Constructeurs
  ExplicitEuler();
  ExplicitEuler(DataFile* DF, Mesh* mesh, Physics* physics, FiniteVolume* finVol);

  // Initialiseur
  void Initialize(DataFile* DF, Mesh* mesh, Physics* physics, FiniteVolume* finVol);

  // One time step
  void oneStep();
};



class RK2: public TimeScheme
{
public:
  // Constructeurs
  RK2();
  RK2(DataFile* DF, Mesh* mesh, Physics* physics, FiniteVolume* finVol);

  // Initialiseur
  void Initialize(DataFile* DF, Mesh* mesh, Physics* physics, FiniteVolume* finVol);

  // One time step
  void oneStep();
};



#endif // TIME_SCHEME_H
