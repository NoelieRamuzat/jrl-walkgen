/*
 * Copyright 2011
 *
 * Andrei Herdt
 *
 * JRL, CNRS/AIST, INRIA Grenoble-Rhone-Alpes
 *
 * This file is part of walkGenJrl.
 * walkGenJrl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * walkGenJrl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Lesser Public License for more details.
 * You should have received a copy of the GNU Lesser General Public License
 * along with walkGenJrl.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/// Simulate a rigid body

#include <PreviewControl/rigid-body-system.hh>

using namespace PatternGeneratorJRL;
using namespace std;

RigidBodySystem::RigidBodySystem( SimplePluginManager *SPM, CjrlHumanoidDynamicRobot *aHS ):
    Mass_(0),CoMHeight_(0),T_(0),Tr_(0),Ta_(0),N_(0),
    OFTG_(0)
{

  OFTG_ = new OnLineFootTrajectoryGeneration(SPM,aHS->leftFoot());

}


RigidBodySystem::~RigidBodySystem()
{

  if (OFTG_!=0)
    delete OFTG_;

}


void
RigidBodySystem::initialize(  )
{

  // Create and initialize online interpolation of feet trajectories
  OFTG_->InitializeInternalDataStructures();
  OFTG_->SetSingleSupportTime(0.7);
  OFTG_->SetDoubleSupportTime(T_);
  OFTG_->QPSamplingPeriod(T_);
  OFTG_->FeetDistance(0.2);

  // Initialize dynamics
  // -------------------
  CoM_.Mass( Mass_ );
  CoM_.NbSamplingsPreviewed( N_ );
  CoM_.initialize();
  compute_dyn_cjerk();
  compute_dyn_cop();

  LeftFoot_.NbSamplingsPreviewed( N_ );
  LeftFoot_.initialize();
  RightFoot_.NbSamplingsPreviewed( N_ );
  RightFoot_.initialize();


}


int
RigidBodySystem::update( const SupportFSM * FSM, support_state_t & CurrentSupport, double Time )
{

  compute_dyn_pol_feet( LeftFoot_.Dynamics(POSITION), RightFoot_.Dynamics(POSITION), FSM, CurrentSupport, Time );
  compute_dyn_pol_feet( LeftFoot_.Dynamics(ACCELERATION), RightFoot_.Dynamics(ACCELERATION), FSM, CurrentSupport, Time );

  return 0;

}


int
RigidBodySystem::compute_dyn_cop( )
{

  bool preserve = true;
  CoPDynamics_.U.resize(N_,N_,!preserve);
  CoPDynamics_.U.clear();
  CoPDynamics_.UT.resize(N_,N_,!preserve);
  CoPDynamics_.UT.clear();
  CoPDynamics_.S.resize(N_,3,!preserve);
  CoPDynamics_.S.clear();
  for(unsigned int i=0;i<N_;i++)
    {
      CoPDynamics_.S(i,0) = 1.0; CoPDynamics_.S(i,1) = (i+1)*T_; CoPDynamics_.S(i,2) = (i+1)*(i+1)*T_*T_*0.5-CoMHeight_/9.81;
      for(unsigned int j=0;j<N_;j++)
        if (j<=i)
          CoPDynamics_.U(i,j) = CoPDynamics_.UT(j,i) = (1 + 3*(i-j) + 3*(i-j)*(i-j)) * T_*T_*T_/6.0 - T_*CoMHeight_/9.81;
        else
          CoPDynamics_.U(i,j) = CoPDynamics_.UT(j,i) = 0.0;
    }

  return 0;

}


int
RigidBodySystem::compute_dyn_cjerk()
{

  // Initialize dynamics
  // -------------------
  compute_dyn_cjerk( CoM_.Dynamics(POSITION) );
  compute_dyn_cjerk( CoM_.Dynamics(VELOCITY) );
  compute_dyn_cjerk( CoM_.Dynamics(ACCELERATION) );
  compute_dyn_cjerk( CoM_.Dynamics(JERK) );

  return 0;

}


int
RigidBodySystem::compute_dyn_cjerk( linear_dynamics_t & Dynamics )
{

  bool preserve = true;
  Dynamics.U.resize(N_,N_,!preserve);
  Dynamics.U.clear();
  Dynamics.UT.resize(N_,N_,!preserve);
  Dynamics.UT.clear();
  Dynamics.S.resize(N_,3,!preserve);
  Dynamics.S.clear();

  switch(Dynamics.Type)
  {
  case POSITION:
    for(unsigned int i=0;i<N_;i++)
      {
        Dynamics.S(i,0) = 1; Dynamics.S(i,1) =(i+1)*T_; Dynamics.S(i,2) = ((i+1)*T_)*((i+1)*T_)/2;
        for(unsigned int j=0;j<N_;j++)
          if (j<=i)
            Dynamics.U(i,j) = Dynamics.UT(j,i) =(1+3*(i-j)+3*(i-j)*(i-j))*(T_*T_*T_)/6 ;
          else
            Dynamics.U(i,j) = Dynamics.UT(j,i) = 0.0;
      }
    break;
  case VELOCITY:
    for(unsigned int i=0;i<N_;i++)
      {
        Dynamics.S(i,0) = 0.0; Dynamics.S(i,1) = 1.0; Dynamics.S(i,2) = (i+1)*T_;
        for(unsigned int j=0;j<N_;j++)
          if (j<=i)
            Dynamics.U(i,j) = Dynamics.UT(j,i) = (2*(i-j)+1)*T_*T_*0.5 ;
          else
            Dynamics.U(i,j) = Dynamics.UT(j,i) = 0.0;
      }
    break;
  case ACCELERATION:
    for(unsigned int i=0;i<N_;i++)
      {
        Dynamics.S(i,0) = 0.0; Dynamics.S(i,1) = 0.0; Dynamics.S(i,2) = 1.0;
        for(unsigned int j=0;j<N_;j++)
          if (j<=i)
            Dynamics.U(i,j) = Dynamics.UT(j,i) = T_;
          else
            Dynamics.U(i,j) = Dynamics.UT(j,i) = 0.0;
      }
    break;
  case JERK:
    for(unsigned int i=0;i<N_;i++)
      {
        Dynamics.S(i,0) = 0.0; Dynamics.S(i,1) = 0.0; Dynamics.S(i,2) = 0.0;
        for(unsigned int j=0;j<N_;j++)
          if (j==i)
            Dynamics.U(i,j) = Dynamics.UT(j,i) = 1.0;
          else
            Dynamics.U(i,j) = Dynamics.UT(j,i) = 0.0;
      }
    break;

  }

  return 0;

}


int
RigidBodySystem::compute_dyn_pol_feet( linear_dynamics_t & LeftFootDynamics, linear_dynamics_t & RightFootDynamics,
    const SupportFSM * FSM, const support_state_t & CurrentSupport, double Time )
{

  // Resize the matrices:
  // --------------------
  support_state_t PreviewedSupport = CurrentSupport;
  PreviewedSupport.StepNumber  = 0;
  reference_t Ref;
  Ref.Global.x = 1.0;
  for(unsigned int i=0;i<N_;i++)
    {
      FSM->set_support_state( Time, i+1, PreviewedSupport, Ref );
    }
  unsigned int NbSteps = PreviewedSupport.StepNumber;

  bool preserve = true;
  LeftFootDynamics.U.resize(N_,NbSteps,!preserve);
  LeftFootDynamics.U.clear();
  LeftFootDynamics.UT.resize(NbSteps,N_,!preserve);
  LeftFootDynamics.UT.clear();
  LeftFootDynamics.S.clear();
  RightFootDynamics.U.resize(N_,NbSteps,!preserve);
  RightFootDynamics.U.clear();
  RightFootDynamics.UT.resize(NbSteps,N_,!preserve);
  RightFootDynamics.UT.clear();
  RightFootDynamics.S.clear();

  // Fill the matrices:
  // ------------------
  linear_dynamics_t * SFDynamics;
  linear_dynamics_t * FFDynamics;
  PreviewedSupport = CurrentSupport;
  PreviewedSupport.StepNumber  = 0;
  double Spbar[3], Sabar[3];
  double Upbar[2], Uabar[2];
  unsigned int NbInstantSS = 0;
  unsigned int SwitchInstant = 0;
  for(unsigned int i=0;i<N_;i++)
    {
      FSM->set_support_state( Time, i, PreviewedSupport, Ref );
      if(PreviewedSupport.Foot == LEFT)
        {
          SFDynamics = & LeftFootDynamics;
          FFDynamics = & RightFootDynamics;
        }
      else
        {
          SFDynamics = & RightFootDynamics;
          FFDynamics = & LeftFootDynamics;
        }
      if(PreviewedSupport.StateChanged == true)
        {
          SwitchInstant = i+1;
          NbInstantSS = 0;
        }
      if(PreviewedSupport.Phase == DS)
        {
          // The previous row is copied in DS phase
          if(i==0)
            {
              if(SFDynamics->Type == POSITION)
                {
                  SFDynamics->S(i,0) = 1.0;FFDynamics->S(i,0) = 1.0;
                  SFDynamics->S(i,1) = 0.0;FFDynamics->S(i,1) = 0.0;
                  SFDynamics->S(i,2) = 0.0;FFDynamics->S(i,2) = 0.0;
                }
            }
          if(i>0)
            {
              SFDynamics->S(i,0) = SFDynamics->S(i-1,0);FFDynamics->S(i,0) = FFDynamics->S(i-1,0);
              SFDynamics->S(i,1) = SFDynamics->S(i-1,1);FFDynamics->S(i,1) = FFDynamics->S(i-1,1);
              SFDynamics->S(i,2) = SFDynamics->S(i-1,2);FFDynamics->S(i,2) = FFDynamics->S(i-1,2);
              for(unsigned int SNb = 0; SNb < NbSteps; SNb++)
                {
                  SFDynamics->U(i,SNb) = SFDynamics->UT(SNb,i) = SFDynamics->U(i-1,SNb);
                  FFDynamics->U(i,SNb) = FFDynamics->UT(SNb,i) = FFDynamics->U(i-1,SNb);
                }
            }
          NbInstantSS = 0;
        }
      else
        {

          compute_sbar( Spbar, Sabar, (NbInstantSS+1)*T_, FSM->StepPeriod()-T_ );
          compute_ubar( Upbar, Uabar, (NbInstantSS+1)*T_, FSM->StepPeriod()-T_ );
          if(PreviewedSupport.StepNumber == 0 && PreviewedSupport.StepNumber < NbSteps)
            {
              if(FFDynamics->Type == POSITION)
                {
                  FFDynamics->S(i,0) = Spbar[0];SFDynamics->S(i,0) = 1.0;
                  FFDynamics->S(i,1) = Spbar[1];SFDynamics->S(i,1) = 0.0;
                  FFDynamics->S(i,2) = Spbar[2];SFDynamics->S(i,2) = 0.0;
                  FFDynamics->U(i,PreviewedSupport.StepNumber) = FFDynamics->UT(PreviewedSupport.StepNumber,i) = Upbar[0];
                  SFDynamics->U(i,PreviewedSupport.StepNumber) = SFDynamics->UT(PreviewedSupport.StepNumber,i) = 0.0;
                }
              else if(FFDynamics->Type == ACCELERATION)
                {
                  FFDynamics->S(i,0) = Sabar[0];SFDynamics->S(i,0) = 0.0;
                  FFDynamics->S(i,1) = Sabar[1];SFDynamics->S(i,1) = 0.0;
                  FFDynamics->S(i,2) = Sabar[2];SFDynamics->S(i,2) = 1.0;
                  FFDynamics->U(i,PreviewedSupport.StepNumber) = FFDynamics->UT(PreviewedSupport.StepNumber,i) = Uabar[0];
                  SFDynamics->U(i,PreviewedSupport.StepNumber) = SFDynamics->UT(PreviewedSupport.StepNumber,i) = 0.0;
                }
              if((NbInstantSS+1)*T_ > FSM->StepPeriod()-T_)
                {
                  FFDynamics->S(i,0) = FFDynamics->S(i-1,0);SFDynamics->S(i,0) = SFDynamics->S(i-1,0);
                  FFDynamics->S(i,1) = FFDynamics->S(i-1,1);SFDynamics->S(i,1) = SFDynamics->S(i-1,1);
                  FFDynamics->S(i,2) = FFDynamics->S(i-1,2);SFDynamics->S(i,2) = SFDynamics->S(i-1,2);
                  FFDynamics->U(i,PreviewedSupport.StepNumber) = FFDynamics->UT(PreviewedSupport.StepNumber,i) = FFDynamics->U(i-1,PreviewedSupport.StepNumber);
                  SFDynamics->U(i,PreviewedSupport.StepNumber) = SFDynamics->UT(PreviewedSupport.StepNumber,i) = SFDynamics->U(i-1,PreviewedSupport.StepNumber);
                }
            }
          else if(PreviewedSupport.StepNumber == 1 && PreviewedSupport.StepNumber < NbSteps)
            {
              if(FFDynamics->Type == POSITION)
                {
                  FFDynamics->S(i,0) = Spbar[0]; //SFDynamics->S(i,0) = SFDynamics->S(i-1,0);
                  FFDynamics->S(i,1) = Spbar[1]; //SFDynamics->S(i,1) = SFDynamics->S(i-1,1);
                  FFDynamics->S(i,2) = Spbar[2]; //SFDynamics->S(i,2) = SFDynamics->S(i-1,2);
                  FFDynamics->U(i,PreviewedSupport.StepNumber) = FFDynamics->UT(PreviewedSupport.StepNumber,i) = Upbar[0];
                  SFDynamics->U(i,PreviewedSupport.StepNumber-1) = SFDynamics->UT(PreviewedSupport.StepNumber-1,i) = 1.0;
                }
              else if(FFDynamics->Type == ACCELERATION)
                {
                  FFDynamics->S(i,0) = Sabar[0];
                  FFDynamics->S(i,1) = Sabar[1];
                  FFDynamics->S(i,2) = Sabar[2];
                  FFDynamics->U(i,PreviewedSupport.StepNumber) = FFDynamics->UT(PreviewedSupport.StepNumber,i) = Uabar[0];
                  SFDynamics->U(i,PreviewedSupport.StepNumber-1) = SFDynamics->UT(PreviewedSupport.StepNumber-1,i) = 1.0;
                }
              if((NbInstantSS+1)*T_ > FSM->StepPeriod()-T_)
                {
                  FFDynamics->S(i,0) = FFDynamics->S(i-1,0);SFDynamics->S(i,0) = SFDynamics->S(i-1,0);
                  FFDynamics->S(i,1) = FFDynamics->S(i-1,1);SFDynamics->S(i,1) = SFDynamics->S(i-1,1);
                  FFDynamics->S(i,2) = FFDynamics->S(i-1,2);SFDynamics->S(i,2) = SFDynamics->S(i-1,2);
                  FFDynamics->U(i,PreviewedSupport.StepNumber) = FFDynamics->UT(PreviewedSupport.StepNumber,i) = FFDynamics->U(i-1,PreviewedSupport.StepNumber);
                  SFDynamics->U(i,PreviewedSupport.StepNumber-1) = SFDynamics->UT(PreviewedSupport.StepNumber-1,i) = SFDynamics->U(i-1,PreviewedSupport.StepNumber-1);
                }
            }
          else if(PreviewedSupport.StepNumber == 2)
            {
              FFDynamics->S(i,0) = FFDynamics->S(i-1,0);SFDynamics->S(i,0) = SFDynamics->S(i-1,0);
              FFDynamics->S(i,1) = FFDynamics->S(i-1,1);SFDynamics->S(i,1) = SFDynamics->S(i-1,1);
              FFDynamics->S(i,2) = FFDynamics->S(i-1,2);SFDynamics->S(i,2) = SFDynamics->S(i-1,2);
              for(unsigned int j = 0; j<NbSteps; j++)
                {
                  FFDynamics->U(i,j) = FFDynamics->UT(j,i) = FFDynamics->U(i-1,j);
                  SFDynamics->U(i,j) = SFDynamics->UT(j,i) = SFDynamics->U(i-1,j);
                }
            }
          NbInstantSS++;
        }
    }

  return 0;

}


int
RigidBodySystem::generate_trajectories( double Time, const support_state_t & CurrentSupport, const solution_t & Result,
    const std::deque<support_state_t> & PrwSupportStates_deq, const std::deque<double> & PreviewedSupportAngles_deq,
    std::deque<FootAbsolutePosition> & LeftFootTraj_deq, std::deque<FootAbsolutePosition> & RightFootTraj_deq )
{

  FootAbsolutePosition CurLeftFoot = LeftFootTraj_deq.back();
  FootAbsolutePosition CurRightFoot = RightFootTraj_deq.back();
  unsigned int NumberStepsPrwd = PrwSupportStates_deq.back().StepNumber;
  OFTG_->interpolate_feet_positions(Time, CurrentSupport,
      Result.Solution_vec[2*N_], Result.Solution_vec[2*N_+NumberStepsPrwd],
      PreviewedSupportAngles_deq,
      LeftFootTraj_deq, RightFootTraj_deq);

  linear_dynamics_t LFDyn = LeftFoot_.Dynamics( ACCELERATION );
//  cout<<"LFDyn.S"<<LFDyn.S<<endl;
//  cout<<"LFDyn.U"<<LFDyn.U<<endl;
//  cout<<"Prediction: "<<LFDyn.S(0,0)*CurLeftFoot.x+LFDyn.S(0,1)*CurLeftFoot.dx+LFDyn.S(0,2)*CurLeftFoot.ddx+LFDyn.U(0,0)*Result.Solution_vec[2*N_]<<endl;
//  cout<<"Reality: "<<LeftFootTraj_deq.back().x<<endl;
  linear_dynamics_t RFDyn = RightFoot_.Dynamics( ACCELERATION );
//  cout<<"CurrentSupport.Phase = "<<CurrentSupport.Phase<<endl;
//  cout<<"CurrentSupport.Foot = "<<CurrentSupport.Foot<<endl;
//  cout<<"CurrentSupport.TimeLimit = "<<CurrentSupport.TimeLimit<<endl;
//  cout<<"RFDyn.S"<<RFDyn.S<<endl;
//  cout<<"RFDyn.U"<<RFDyn.U<<endl;
//  cout<<"Result.Solution_vec[2*N_] = "<<Result.Solution_vec[2*N_]<<endl;
//  cout<<"Prediction: "<<RFDyn.S(0,0)*CurRightFoot.x+RFDyn.S(0,1)*CurRightFoot.dx+RFDyn.S(0,2)*CurRightFoot.ddx+RFDyn.U(0,0)*Result.Solution_vec[2*N_]
//<<"Prediction: "<<LFDyn.S(0,0)*CurRightFoot.x+LFDyn.S(0,1)*CurRightFoot.dx+LFDyn.S(0,2)*CurRightFoot.ddx+LFDyn.U(0,0)*Result.Solution_vec[2*N_]<<endl;
//  cout<<"Prediction: "<<RFDyn.S(1,0)*CurRightFoot.x+RFDyn.S(1,1)*CurRightFoot.dx+RFDyn.S(1,2)*CurRightFoot.ddx+RFDyn.U(1,0)*Result.Solution_vec[2*N_]
//<<"Prediction: "<<LFDyn.S(1,0)*CurRightFoot.x+LFDyn.S(1,1)*CurRightFoot.dx+LFDyn.S(1,2)*CurRightFoot.ddx+LFDyn.U(1,0)*Result.Solution_vec[2*N_]<<endl;
//  cout<<"Prediction: "<<RFDyn.S(2,0)*CurRightFoot.x+RFDyn.S(2,1)*CurRightFoot.dx+RFDyn.S(2,2)*CurRightFoot.ddx+RFDyn.U(2,0)*Result.Solution_vec[2*N_]
//<<"Prediction: "<<LFDyn.S(2,0)*CurRightFoot.x+LFDyn.S(2,1)*CurRightFoot.dx+LFDyn.S(2,2)*CurRightFoot.ddx+LFDyn.U(2,0)*Result.Solution_vec[2*N_]<<endl;
//  cout<<"Prediction: "<<RFDyn.S(3,0)*CurRightFoot.x+RFDyn.S(3,1)*CurRightFoot.dx+RFDyn.S(3,2)*CurRightFoot.ddx+RFDyn.U(3,0)*Result.Solution_vec[2*N_]
//  <<"Prediction: "<<LFDyn.S(3,0)*CurRightFoot.x+LFDyn.S(3,1)*CurRightFoot.dx+LFDyn.S(3,2)*CurRightFoot.ddx+LFDyn.U(3,0)*Result.Solution_vec[2*N_]<<endl;
//  cout<<"Prediction: "<<RFDyn.S(4,0)*CurRightFoot.x+RFDyn.S(4,1)*CurRightFoot.dx+RFDyn.S(4,2)*CurRightFoot.ddx+RFDyn.U(4,0)*Result.Solution_vec[2*N_]
//<<"Prediction: "<<LFDyn.S(4,0)*CurRightFoot.x+LFDyn.S(4,1)*CurRightFoot.dx+LFDyn.S(4,2)*CurRightFoot.ddx+LFDyn.U(4,0)*Result.Solution_vec[2*N_]<<endl;
//  cout<<"Prediction: "<<RFDyn.S(5,0)*CurRightFoot.x+RFDyn.S(5,1)*CurRightFoot.dx+RFDyn.S(5,2)*CurRightFoot.ddx+RFDyn.U(5,0)*Result.Solution_vec[2*N_]
//  <<"Prediction: "<<LFDyn.S(5,0)*CurRightFoot.x+LFDyn.S(5,1)*CurRightFoot.dx+LFDyn.S(5,2)*CurRightFoot.ddx+LFDyn.U(5,0)*Result.Solution_vec[2*N_]<<endl;
//  cout<<"Prediction: "<<RFDyn.S(6,0)*CurRightFoot.x+RFDyn.S(6,1)*CurRightFoot.dx+RFDyn.S(6,2)*CurRightFoot.ddx+RFDyn.U(6,0)*Result.Solution_vec[2*N_]
//  <<"Prediction: "<<LFDyn.S(6,0)*CurRightFoot.x+LFDyn.S(6,1)*CurRightFoot.dx+LFDyn.S(6,2)*CurRightFoot.ddx+LFDyn.U(6,0)*Result.Solution_vec[2*N_]<<endl;
//  cout<<"Prediction: "<<RFDyn.S(7,0)*CurRightFoot.x+RFDyn.S(7,1)*CurRightFoot.dx+RFDyn.S(7,2)*CurRightFoot.ddx+RFDyn.U(7,0)*Result.Solution_vec[2*N_]
//  <<"Prediction: "<<LFDyn.S(7,0)*CurRightFoot.x+LFDyn.S(7,1)*CurRightFoot.dx+LFDyn.S(7,2)*CurRightFoot.ddx+LFDyn.U(7,0)*Result.Solution_vec[2*N_]<<endl;
//  cout<<"Prediction: "<<RFDyn.S(8,0)*CurRightFoot.x+RFDyn.S(8,1)*CurRightFoot.dx+RFDyn.S(8,2)*CurRightFoot.ddx+RFDyn.U(8,0)*Result.Solution_vec[2*N_]
//  <<"Prediction: "<<LFDyn.S(8,0)*CurRightFoot.x+LFDyn.S(8,1)*CurRightFoot.dx+LFDyn.S(8,2)*CurRightFoot.ddx+LFDyn.U(8,0)*Result.Solution_vec[2*N_]<<endl;
//  cout<<"Prediction: "<<RFDyn.S(9,0)*CurRightFoot.x+RFDyn.S(9,1)*CurRightFoot.dx+RFDyn.S(9,2)*CurRightFoot.ddx+RFDyn.U(9,0)*Result.Solution_vec[2*N_]
//  <<"Prediction: "<<LFDyn.S(9,0)*CurRightFoot.x+LFDyn.S(9,1)*CurRightFoot.dx+LFDyn.S(9,2)*CurRightFoot.ddx+LFDyn.U(9,0)*Result.Solution_vec[2*N_]<<endl;
//  cout<<"Prediction: "<<RFDyn.S(10,0)*CurRightFoot.x+RFDyn.S(10,1)*CurRightFoot.dx+RFDyn.S(10,2)*CurRightFoot.ddx+RFDyn.U(10,0)*Result.Solution_vec[2*N_]
//  <<"Prediction: "<<LFDyn.S(10,0)*CurRightFoot.x+LFDyn.S(10,1)*CurRightFoot.dx+LFDyn.S(10,2)*CurRightFoot.ddx+LFDyn.U(10,0)*Result.Solution_vec[2*N_]<<endl;
//  cout<<"Prediction: "<<RFDyn.S(11,0)*CurRightFoot.x+RFDyn.S(11,1)*CurRightFoot.dx+RFDyn.S(11,2)*CurRightFoot.ddx+RFDyn.U(11,0)*Result.Solution_vec[2*N_]
//  <<"Prediction: "<<LFDyn.S(11,0)*CurRightFoot.x+LFDyn.S(11,1)*CurRightFoot.dx+LFDyn.S(11,2)*CurRightFoot.ddx+LFDyn.U(11,0)*Result.Solution_vec[2*N_]<<endl;
//  cout<<"Prediction: "<<RFDyn.S(12,0)*CurRightFoot.x+RFDyn.S(12,1)*CurRightFoot.dx+RFDyn.S(12,2)*CurRightFoot.ddx+RFDyn.U(12,0)*Result.Solution_vec[2*N_]
//  <<"Prediction: "<<LFDyn.S(12,0)*CurRightFoot.x+LFDyn.S(12,1)*CurRightFoot.dx+LFDyn.S(12,2)*CurRightFoot.ddx+LFDyn.U(12,0)*Result.Solution_vec[2*N_]<<endl;
//  cout<<"Prediction: "<<RFDyn.S(13,0)*CurRightFoot.x+RFDyn.S(13,1)*CurRightFoot.dx+RFDyn.S(13,2)*CurRightFoot.ddx+RFDyn.U(13,0)*Result.Solution_vec[2*N_]
//  <<"Prediction: "<<LFDyn.S(13,0)*CurRightFoot.x+LFDyn.S(13,1)*CurRightFoot.dx+LFDyn.S(13,2)*CurRightFoot.ddx+LFDyn.U(13,0)*Result.Solution_vec[2*N_]<<endl;
//  cout<<"Reality: "<<RightFootTraj_deq.back().ddx<<endl;

  return 0;

}


//void
//RigidBodySystem::increment_state(double Control)
//{
//
//}


//
// Private methods:
//

int
RigidBodySystem::compute_sbar( double * Spbar, double * Sabar, double T, double Td )
{

  double Td2 = Td*Td;
  double Td3 = Td*Td*Td;
  double Td4 = Td*Td*Td*Td;
  double Td5 = Td*Td*Td*Td*Td;

  Spbar[0] = 1.0; Spbar[1] = T; Spbar[2] = 0.0;
  double Ttemp = 0.5*T*T;
  Spbar[2] += Ttemp;
  Ttemp *= T;
  Spbar[0] -= 20.0/Td3*Ttemp; Spbar[1] -= 12.0/Td2*Ttemp; Spbar[2] -= 3.0/Td*Ttemp;
  Ttemp *= T;
  Spbar[0] += 30.0/Td4*Ttemp; Spbar[1] += 16.0/Td3*Ttemp; Spbar[2] += 3.0/Td2*Ttemp;
  Ttemp *= T;
  Spbar[0] -= 12.0/Td5*Ttemp; Spbar[1] -= 6.0/Td4*Ttemp; Spbar[2] -= 1.0/Td3*Ttemp;

  Sabar[0] = Sabar[1] = 0.0; Sabar[2] = 1.0;
  Ttemp = 0.5;
  Ttemp *= T;
  Sabar[0] -= 6.0*20.0/Td3*Ttemp; Sabar[1] -= 6.0*12.0/Td2*Ttemp; Sabar[2] -= 6.0*3.0/Td*Ttemp;
  Ttemp *= T;
  Sabar[0] += 12.0*30.0/Td4*Ttemp; Sabar[1] += 12.0*16.0/Td3*Ttemp; Sabar[2] += 12.0*3.0/Td2*Ttemp;
  Ttemp *= T;
  Sabar[0] -= 20.0*12.0/Td5*Ttemp; Sabar[1] -= 20.0*6.0/Td4*Ttemp; Sabar[2] -= 20.0*1.0/Td3*Ttemp;

  return 0;

}


int
RigidBodySystem::compute_ubar( double * Upbar, double * Uabar, double T, double Td )
{

  double Td3 = Td*Td*Td;
  double Td4 = Td*Td*Td*Td;
  double Td5 = Td*Td*Td*Td*Td;

  Upbar[0] =  0.0; //Upbar[1] = 0.0;
  double Ttemp = T*T*T;
  Upbar[0] += 10.0/Td3*Ttemp; //Upbar[1] += 10.0/Td3*Ttemp;
  Ttemp *= T;
  Upbar[0] -= 15.0/Td4*Ttemp; //Upbar[1] -= 15.0/Td4*Ttemp;
  Ttemp *= T;
  Upbar[0] += 6.0/Td5*Ttemp; //Upbar[1] += 6.0/Td5*Ttemp;

  Uabar[0] =  0.0;// Uabar[1] = 0.0;
  Ttemp = T;
  Uabar[0] += 6.0*10.0/Td3*Ttemp;// Uabar[1] += 6.0*10.0/Td3*Ttemp;
  Ttemp *= T;
  Uabar[0] -= 12.0*15.0/Td4*Ttemp;// Uabar[1] -= 12.0*15.0/Td4*Ttemp;
  Ttemp *= T;
  Uabar[0] += 20.0*6.0/Td5*Ttemp;// Uabar[1] += 20.0*6.0/Td5*Ttemp;

  return 0;

}


// ACCESSORS:
// ----------
linear_dynamics_t const &
RigidBodySystem::Dynamics( ) const
{

  return CoPDynamics_;

}

linear_dynamics_t &
RigidBodySystem::Dynamics()
{

  return CoPDynamics_;

}