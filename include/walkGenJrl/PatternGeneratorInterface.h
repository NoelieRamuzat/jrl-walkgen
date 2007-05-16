/** @doc  This object provides a unified interface to access the pattern generator.
    It allows to hide all the computation and hacking to the user.

    SVN Information:
   $Id$
   $Author$
   $Date$
   $Revision $
   $Source $
   $Log $


   Copyright (c) 2005-2006, 
   @author Olivier Stasse
   
   JRL-Japan, CNRS/AIST

   All rights reserved.
   
   Redistribution and use in source and binary forms, with or without modification, 
   are permitted provided that the following conditions are met:
   
   * Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
   * Neither the name of the <ORGANIZATION> nor the names of its contributors 
   may be used to endorse or promote products derived from this software without specific prior written permission.
   
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS 
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
   AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
   OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
   STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
   IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _PATTERN_GENERATOR_INTERFACE_H_
#define _PATTERN_GENERATOR_INTERFACE_H_

#include <sstream>
#include <MatrixAbstractLayer/MatrixAbstractLayer.h>

#include <PreviewControl/ZMPPreviewControlWithMultiBodyZMP.h>
#include <PreviewControl/PreviewControl.h>

#include <ZMPRefTrajectoryGeneration/ZMPDiscretization.h>

#include <MotionGeneration/ComAndFootRealizationByGeometry.h>
#include <MotionGeneration/GenerateMotionFromKineoWorks.h>
#include <MotionGeneration/StepOverPlanner.h>

#include <StepStackHandler.h>
#include <robotDynamicsJRLJapan/HumanoidDynamicMultiBody.h>

#include <SimplePluginManager.h>

namespace PatternGeneratorJRL
{
  class SimplePlugin;

  /** @ingroup Interface
      This class is the interface between the Pattern Generator and the 
      external world. In addition to the classical setter and getter for various parameters
      there is the possibility to pass commands a string of stream to the method
      \a ParseCmd().
   */
  class PatternGeneratorInterface : public SimplePluginManager
  {    
  public:
    
    /*! Constructor 
      @param strm: Should provide the file to initialize the preview control,
      the path to the VRML model, and the name of the file containing the VRML model.
     */
    PatternGeneratorInterface(std::istringstream &strm);

    /*! Destructor */
    ~PatternGeneratorInterface();

    /*! Set the gain factor for the default behavior of the arm */
    void m_SetArmParameters(std::istringstream &strm);

    /*! Set obstacles parameters for stepping over */
    void m_SetObstacleParameters(std::istringstream &strm);

    /*! Set the shift of the ZMP height for stepping over. */
    void m_SetZMPShiftParameters(std::istringstream &strm);
    
    /*! Set time distribution parameters. */
    void m_SetTimeDistrParameters(std::istringstream &strm);

    /*! Set upper body motion parameters. */
    void m_SetUpperBodyMotionParameters(std::istringstream &strm);

    /*! Set the limits of the feasibility (stepping over parameters) */
    void m_SetLimitsFeasibility(std::istringstream &strm);

    /*! Set the walking mode: 
      0: Normal mode.
      1: Waist height variation.
      3: Kineo works mode.
      4: Following the upper-body motion provided by another plugin. */
    void m_WhichWalkMode(std::istringstream &strm);

    /*! Read file from Kineoworks. */
    void m_ReadFileFromKineoWorks(std::istringstream &strm);

    /*! Specify a sequence of step without asking for 
      immediate execution and end sequence. */
    void m_PartialStepSequence(istringstream &strm);

    // This method creates an arc on the step sequence stack.
    // (Just an interface)
    void m_CreateArcInStepStack(std::istringstream &strm);

    // This method creates a centered arc on the step sequence stack.
    // (Just an interface)
    void m_CreateArcCenteredInStepStack(std::istringstream &strm);

    // This method the constraints on the foot dimension for PBW's algorithm
    void m_SetPBWConstraint(istringstream &strm);

    // This method set PBW's algorithm for ZMP trajectory planning.
    void m_SetAlgoForZMPTraj(istringstream &strm);

    // This methods is doing the real job for the arc,
    // but does not prepare for the support foot.
    void CreateArcInStepStack(  double x,
				double y,
				double R,
				double arc_deg,
				int SupportFoot);

    // This methods is doing the real job for the arc centered,
    // but does not prepare for the support foot.
    void CreateArcCenteredInStepStack( double R,
				       double arc_deg,
				       int SupportFoot);

    // This method is doing the real job for preparing the
    // support foot.
    void PrepareForSupportFoot(int SupportFoot);

    // Interface to hrpsys to start the realization of 
    // the stacked of step sequences.
    void m_FinishAndRealizeStepSequence(std::istringstream &strm);

    // The method which is really creating the buffer 
    // related to the stacked step sequences.
    void FinishAndRealizeStepSequence();

    // Prepare to start or stop on a given support foot.
    // (Just an interface)
    void m_PrepareForSupportFoot(std::istringstream &strm);

    // This method is finishing on the last support foot 
    // after the last motion.
    void m_FinishOnTheLastCorrectSupportFoot(std::istringstream &strm);
    
    /// Realize a sequence of steps.
    void m_StepSequence(std::istringstream &strm);

    /// Set the angle for lift off and landing of the foot.
    void m_SetOmega(std::istringstream &strm);

    /// Set the maximal height of the foot trajectory.
    void m_SetStepHeight(std::istringstream &strm);

    // Set the single support time.
    void m_SetSingleSupportTime(std::istringstream &strm);

    // Set the double support time.
    void m_SetDoubleSupportTime(std::istringstream &strm);
    
    
    // Set a general parse command (to be used out of a plugin).
    int ParseCmd(std::istringstream &strm);

    // Run One Step of the global control loop
    // aka The Main Method To Be Used.
    // @param : CurrentConfiguration: The current configuration of the robot according to 
    // the implementation of dynamic-JRLJapan. This should be first position and orientation
    // of the waist, and then all the DOFs of your robot. 
    // @param : CurrentVelocity:  The current velocity of the robot according to the 
    // the implementation of dynamic-JRLJapan. 
    // @param : ZMPTarget : The target ZMP in the waist reference frame.
    // @return: True is there is still some data to send, false otherwise.
    bool RunOneStepOfTheControlLoop(MAL_VECTOR(,double) & CurrentConfiguration,
				    MAL_VECTOR(,double) & CurrentVelocity,
				    MAL_VECTOR( &ZMPTarget,double));

    // Debug control loop.
    void DebugControlLoop(MAL_VECTOR(,double) & CurrentConfiguration,
			  MAL_VECTOR(,double) & CurrentVelocity,
			  int localindex);

    /*! Set the current joint values of the robot.
      This method is used to properly initialize the pattern generator.
      It also updates the state of the robot if other control mechanisms 
      modifies the upper body part and if this should be taken into account
      into the pattern generator in the second loop of control. */
    void SetCurrentJointValues(MAL_VECTOR( &lCurrentJointValues,double));

    /*! Returns the walking mode. */
    int GetWalkMode();
    
    /*! Get the leg joint velocity */
    void GetLegJointVelocity(MAL_VECTOR( &dqr,double), 
			     MAL_VECTOR( &dql,double));

    /*! Start the creation of steps on line. */
    void StartOnLineStepSequencing();

    /*! Start the creation of steps on line (istringstream interface). */
    void m_StartOnLineStepSequencing(istringstream & strm);
    
    /*! Stop the creation of steps on line. */
    void StopOnLineStepSequencing();

    /*! Stop the creation of steps on line (istringstream interface). */
    void m_StopOnLineStepSequencing(istringstream &strm2);

    /*! Common Initialization of walking. 
     * @return lStartingCOMPosition: For the starting position on the articular space, returns
     the COM position.
     * @return BodyAnglesIni: Takes the initialization values of the robot.
     * @return InitLeftFootAbsPos: Returns the current absolute position of the left foot for
     the given posture of the robot.
     * @return InitRightFootAbsPos: Returns the current absolute position of the right foot
     for the given posture of the robot.
     * @param ClearStepStackHandler: Clean the stack of steps after copy.
     */
    void CommonInitializationOfWalking(MAL_S3_VECTOR(  & lStartingCOMPosition,double),
				       MAL_VECTOR(  & BodyAnglesIni,double),
				       FootAbsolutePosition & InitLeftFootAbsPos, 
				       FootAbsolutePosition & InitRightFootAbsPos,
				       deque<RelativeFootPosition> & lRelativeFootPositions,
				       vector<double> & lCurrentJointValues,
				       bool ClearStepStackHandler);

    /*! Expansion of the buffers handling Center of Masse positions,
      as well as Upper Body Positions. */
    void ExpandCOMAndUpperBodyPositionsQueues(int aNumber);

    /*! Compute the COM, left and right foot position for a given BodyAngle position */
    void EvaluateStartingCOM(MAL_VECTOR(  & Configuration,double),
			     MAL_S3_VECTOR(  & lStartingCOMPosition,double));
    
    
    /*! Add an online step */
    void AddOnLineStep(double X, double Y, double Theta);

    /// For SLAM
    /*! Update the current waist absolute position */
    void UpdateAbsolutePosition(bool UpdateAbsMotionOrNot);

    /*! Get the waist position and orientation as a quaternion,
     and the planar X-Y orientation in Orientation. */
    void getWaistPositionAndOrientation(double TQ[7],double &Orientation);

    /*! Set Waist position and Orientation */
    void setWaistPositionAndOrientation(double TQ[7]);

    /* Get Waist velocity */
    void getWaistVelocity(double &dx,
			  double &dy,
			  double &omega) ;

    /*! An other method to get the waist position using a matrix. */
    void getWaistPositionMatrix(MAL_S4x4_MATRIX( &lWaistAbsPos,double));
     

    /*! \name Methods related to the handling of the objects contained 
      inside this class.  
      @{
    */
    
    /*! Objects instanciation. */
    void ObjectsInstanciation(string & HumanoidSpecificitiesFileName);


    /*! Inter object relations initialization. */
    void InterObjectRelationInitialization(string & PCParametersFileName,
					   string & HumanoidVRMLFileDirectory,
					   string & HumanoidVRMLFileName,
					   string & HumanoidLinkJointRank);

    /*! @}*/

    /*! System to call a given method based on registration of a method. 
      @{
     */
    /*! \brief This method register a method to a specific
      object which derivates from SimplePlugin class 
    */
    bool RegisterMethod(string &MethodName, SimplePlugin *aSP);

    /*! @} */

  private:

    /*! Object to handle the stack of relative steps. */
    StepStackHandler *m_StepStackHandler;

    /*! Buffer needed to perform the stepping over
      obstacle. */
    vector<double> m_ZMPShift;
    
    /*! Gain factor for the default arm motion while walking. */
    double m_GainFactor;

    /*! Object to generate a ZMP profile from
      the step of stacks. It provides a buffer for
      the ZMP position to be used every dt
      in the control loop. */
    ZMPDiscretization *m_ZMPD;

    /*! The Preview Control object. */
    PreviewControl *m_PC;
    
    /*! The multibody objects. */
    DynamicMultiBody *m_DMB,*m_2DMB;

    /*! The object to be used to perform one step of
      control, and generates the corrected CoM trajectory. */
    ZMPPreviewControlWithMultiBodyZMP *m_ZMPpcwmbz;

    /*! Vector for the COM position. */
    deque<COMPosition> m_COMBuffer;
    
    /*! Object needed to perform a path provided by
      Kineo */
    GenerateMotionFromKineoWorks *m_GMFKW;
    
    /*! Buffer of upper body position related to a plan */
    deque<PatternGeneratorJRL::KWNode > m_UpperBodyPositionsBuffer;

    /*! Conversion between the index of the plan and the robot DOFs. */
    vector<int> m_ConversionForUpperBodyFromLocalIndexToRobotDOFs;
    
    /*! Current Actuated Joint values of the robot. */
    vector<double> m_CurrentActuatedJointValues;

    /*! Position of the waist: 
      Relative.*/
    MAL_S4x4_MATRIX( m_WaistRelativePos,double);
    
    /*! Absolute: */
    MAL_S4x4_MATRIX( m_WaistAbsPos,double);
    
    /*! Orientation: */
    double m_AbsTheta, m_AbsMotionTheta;
    
    /*! Position of the motion: */
    MAL_S4x4_MATRIX( m_MotionAbsPos,double);
    MAL_S4x4_MATRIX( m_MotionAbsOrientation,double);
    
    /*! Absolute speed:*/
    MAL_S4_VECTOR( m_AbsLinearVelocity,double);
    MAL_S4_VECTOR( m_AbsAngularVelocity,double);

    /*! Aboluste acceleration */
    MAL_S4_VECTOR( m_AbsLinearAcc,double);
    
    /*! Keeps track of the last correct support foot. */
    int m_KeepLastCorrectSupportFoot;
    
    /*! Boolean to ensure a correct initialization of the 
      step's stack. */
    bool m_IncorrectInitialization;

    /*! Buffer of ZMP positions */
    deque<ZMPPosition> m_ZMPPositions;

    /*! Buffer of Absolute foot position (World frame) */
    deque<FootAbsolutePosition> m_FootAbsolutePositions;

    /*! Buffer of absolute foot position. */
    deque<FootAbsolutePosition> m_LeftFootPositions,m_RightFootPositions;
    
    /*! Buffer of absolute Hand position. */
    deque<FootAbsolutePosition> m_LeftHandPositions,m_RightHandPositions;
    
    /* \brief Methods related to upper body motion. 
       @{
     */

    /*! Index of Upper body joints. */
    vector<int> m_UpperBodyJoints;

    /*! Number of upper body joints. */
    int m_NbOfUpperBodyJoints;

    /* @} */
    /*! Store the debug mode. */
    int m_DebugMode;

    /*! Store the number of degree of freedoms */
    int m_DOF;
    
    /*! Store the height of the arm. */
    double m_ZARM;


    /**! Local copy of Preview Control parameters. */
    /*! Sampling period of the control loop. */
    double m_SamplingPeriod;

    /*! Window of the preview control */
    double m_PreviewControlTime;

    /*! Height of the CoM. */
    double m_Zc;

    /*! Store the local Single support time,
      and the Double support time. */
    float m_TSsupport, m_TDsupport;

    /*! Discrete size of the preview control window */
    unsigned int m_NL;
    
    /*! Local time while performing the control loop. */
    unsigned long int m_count;
    

    /*! Maximal value for the arms in front of the robot */
    double m_Xmax;      

    /*! Variables used to compute speed for other purposes. */
    MAL_VECTOR( m_prev_qr,double);
    MAL_VECTOR( m_prev_ql,double);
    MAL_VECTOR( m_prev_dqr, double);
    MAL_VECTOR( m_prev_dql, double);


    /* Debug variables. */
    MAL_VECTOR( m_Debug_prev_qr, double);
    MAL_VECTOR( m_Debug_prev_ql,double);
    MAL_VECTOR( m_Debug_prev_dqr, double);
    MAL_VECTOR( m_Debug_prev_dql,double);
    MAL_VECTOR( m_Debug_prev_UpperBodyAngles,double);
    MAL_VECTOR( m_Debug_prev_qr_RefState, double);
    MAL_VECTOR( m_Debug_prev_ql_RefState,double);

    double m_Debug_prev_qWaistYaw, m_Debug_prev_dqWaistYaw;
    MAL_S3_VECTOR(,double) m_Debug_prev_P, m_Debug_prev_L;
    bool m_FirstPrint, m_FirstRead;

    bool m_ShouldBeRunning;

    /*! To handle a new step. */
    bool m_NewStep;
    double m_NewStepX, m_NewStepY, m_NewTheta;

    /* ! Store the current relative state of the waist */
    COMPosition m_CurrentWaistState;
    
    /* ! current time period for the control */
    double m_dt;

    /*! Constraint on X and Y */
    double m_ConstraintOnX, m_ConstraintOnY;

    /*! Sampling of the QP. */
    double m_QP_T;
    
    /*! Preview window */
    unsigned int m_QP_N;

    /*! Boolean to use PBW ZMP planner. */
    unsigned char m_BoolPBWAlgo;

    /*! Humanoid Dynamic robot */
    HumanoidDynamicMultiBody * m_HumanoidDynamicRobot, * m_2HumanoidDynamicRobot;

    /*! Speed of the leg. */
    MAL_VECTOR(,double) m_dqr,m_dql;

    /*! Objet to realize the generate the posture for given CoM
      and feet positions. */
    ComAndFootRealization * m_ComAndFootRealization;

    /* \name Object related to stepping over. 
       @{
     */
    
    /*! Planner for stepping over an obstacle. */
    StepOverPlanner *m_StOvPl;

    /*! Position and parameters related to the obstacle. */
    ObstaclePar m_ObstaclePars;
    
    /*! Boolean on the obstacle's detection */
    bool m_ObstacleDetected;	

    /*! Time Distribution factor */
    vector<double> m_TimeDistrFactor;

    /*! Variable for delta feasibility limit */
    double m_DeltaFeasibilityLimit;	
    
    /* @} */



  };

};


#endif
