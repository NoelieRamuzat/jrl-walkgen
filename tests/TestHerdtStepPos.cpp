/*
 * Copyright 2010, 
 *
 * Andrei Herdt
 * Olivier Stasse
 *
 * JRL, CNRS/AIST
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
 *  Research carried out within the scope of the 
 *  Joint Japanese-French Robotics Laboratory (JRL)
 */
/* \file This file tests A. Herdt's walking algorithm for
 * automatic foot placement giving an instantaneous CoM velocity reference.
 */
#include "CommonTools.h"
#include "TestObject.h"

using namespace::PatternGeneratorJRL;
using namespace::PatternGeneratorJRL::TestSuite;
using namespace std;

enum Profiles_t {
  PROFIL_HERDT_ONLINE_WALKING                 // 1
  ,PROFIL_HERDT_STEP_POS_ONLINE_WALKING
};

class TestHerdtStepPos: public TestObject
{

private:
public:
  TestHerdtStepPos(int argc, char *argv[], string &aString, int TestProfile):
    TestObject(argc,argv,aString)
  {
    m_TestProfile = TestProfile;
  };

protected:




  void startOnLineWalking(PatternGeneratorInterface &aPGI)
  {
    CommonInitialization(aPGI);

	{

		istringstream strm2(":setstepspositions   \
							  0.1	0.0	 0.0  \
							  0.2	0.0	 0.0  \
							 -0.2	0.0	 0.0  \
							 -0.1	0.0	 0.0  \
							  0.2	0.0	 0.0  \
							  0.2	0.0	 0.0  \
							  0.2	0.0	 0.0  \
							  0.3	0.0	 0.0  \
							  0.35	0.0	 0.0");
		aPGI.ParseCmd(strm2);
	}
    {
      istringstream strm2(":SetAlgoForZmpTrajectory HerdtStepPos");
      aPGI.ParseCmd(strm2);

    }
    {
      istringstream strm2(":singlesupporttime 0.7");
      aPGI.ParseCmd(strm2);
    }
    {
	      istringstream strm2(":doublesupporttime 0.1");
      aPGI.ParseCmd(strm2);
    }
    {
      istringstream strm2(":HerdtOnlineStepPos 0.2 0.0 0.0");
      aPGI.ParseCmd(strm2);
    }
    {
      istringstream strm2(":numberstepsbeforestop 2");
      aPGI.ParseCmd(strm2);
    }
  }
  
 void startTurningLeft(PatternGeneratorInterface &aPGI)
  {
    {
      istringstream strm2(":setVelReference  0.2 0.0 0.1");
      aPGI.ParseCmd(strm2);
    }
  }
  void startTurningRight(PatternGeneratorInterface &aPGI)
  {
    {
      istringstream strm2(":setVelReference  0.2 0.0 -0.1");
      aPGI.ParseCmd(strm2);
    }
  }
  void stopOnLineWalking(PatternGeneratorInterface &aPGI)
  {
    {
      istringstream strm2(":setVelReference  0.0 0.0 0.0");
      aPGI.ParseCmd(strm2);
    }
  }

  void chooseTestProfile()
  {

    switch(m_TestProfile)
      {

      case PROFIL_HERDT_ONLINE_WALKING:
	startOnLineWalking(*m_PGI);
		case PROFIL_HERDT_STEP_POS_ONLINE_WALKING:
	startOnLineWalking(*m_PGI);
	break;
      default:
	throw("No correct test profile");
	break;
      }
  }

  void generateEvent()
  {
        unsigned TurningLeftTime = 5*200;
    unsigned TurningRightTime = 10*200;
    unsigned StoppingTime = 15*200;

    if (m_OneStep.NbOfIt>TurningLeftTime)
      {
      startTurningLeft(*m_PGI);
      }
    if (m_OneStep.NbOfIt>TurningRightTime)
      {
      startTurningRight(*m_PGI);
      }

    if (m_OneStep.NbOfIt>StoppingTime) 
      {
		stopOnLineWalking(*m_PGI);
      }
  }
};

int PerformTests(int argc, char *argv[])
{

  const unsigned NbTests = 1;
  std::string TestNames[NbTests] = { "TestHerdtStepPosOnLine" };
  int TestProfiles[NbTests] = { PROFIL_HERDT_STEP_POS_ONLINE_WALKING };

  for (unsigned i=0;i<NbTests;i++)
    {
      TestHerdtStepPos aTH2010(argc,argv,
			    TestNames[i],
			    TestProfiles[i]);
      try
	{
	  if (!aTH2010.doTest(std::cout))
	    {
	      cout << "Test nb. " << i+1 <<" (of " << NbTests <<" test(s)) failed!!!"<< endl;
	      return -1;
	    }
	  else
	    cout << "Test nb. " << i+1 <<" (of " << NbTests <<" test(s)) passed."<< endl;
	}
      catch (const char * astr)
	{ cerr << "Failed on following error " << astr << std::endl;
	  return -1; }
    }
  return 0;
}

int main(int argc, char *argv[])
{
  try
    {
      return PerformTests(argc,argv);
    }
  catch (const std::string& msg)
    {
      std::cerr << msg << std::endl;
    }
  return 1;
}


