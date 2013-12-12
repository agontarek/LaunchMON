/*
 * $Header: $
 *--------------------------------------------------------------------------------
 * Copyright (c) 2008, Lawrence Livermore National Security, LLC. Produced at 
 * the Lawrence Livermore National Laboratory. Written by Dong H. Ahn <ahn1@llnl.gov>. 
 * LLNL-CODE-409469. All rights reserved.
 *
 * This file is part of LaunchMON. For details, see 
 * https://computing.llnl.gov/?set=resources&page=os_projects
 *
 * Please also read LICENSE.txt -- Our Notice and GNU Lesser General Public License.
 *
 * 
 * This program is free software; you can redistribute it and/or modify it under the 
 * terms of the GNU General Public License (as published by the Free Software 
 * Foundation) version 2.1 dated February 1999.

 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the terms and conditions of the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along 
 * with this program; if not, write to the Free Software Foundation, Inc., 59 Temple 
 * Place, Suite 330, Boston, MA 02111-1307 USA
 *--------------------------------------------------------------------------------
 *
 *
 *  Update Log:
 *        Dec 12 2013 DHA: Added signal handler support for new launchmon
 *        Dec 07 2012 DHA: Made the false condition of init_rm_instance fatal
 *        Jun 30 2010 DHA: Added faster parse error detection support
 *        Feb 09 2008 DHA: Added LLNS Copyright
 *        Jul 05 2006 DHA: Added signal handling support
 *        Jul 05 2006 DHA: Added self tracing support
 *        Jun 08 2006 DHA: Added attach-to-a-running job support
 *        Jun 08 2006 DHA: Added process_args support
 *        Jan 08 2006 DHA: Created file.          
 */ 

#ifndef SDBG_BASE_DRIVER_IMPL_HXX
#define SDBG_BASE_DRIVER_IMPL_HXX 1

#ifndef HAVE_LAUNCHMON_CONFIG_H
#include "config.h"
#endif

#include "sdbg_opt.hxx"
#include "sdbg_base_tracer.hxx"
#include "sdbg_event_manager.hxx"
#include "sdbg_event_manager_impl.hxx"
#include "sdbg_rm_map.hxx"
#include "sdbg_base_driver.hxx"
#include "sdbg_signal_hlr.hxx"
#include "sdbg_signal_hlr_impl.hxx"
#include "sdbg_rmapi_signal_hlr.hxx"
#include "sdbg_rmapi_signal_hlr_impl.hxx"


////////////////////////////////////////////////////////////////////
//
// PRIVATE copy constructor and operator= (class driver_base_t<>)
//
////////////////////////////////////////////////////////////////////


//!
/*!  driver_base_t<> constructors

*/
template <SDBG_DEFAULT_TEMPLATE_WIDTH> 
driver_base_t<SDBG_DEFAULT_TEMPLPARAM>::driver_base_t
(const driver_base_t &d) : evman(NULL), lmon(NULL)
{
  // this copy construct doesn't copy evman and lmon
  MODULENAME = d.MODULENAME;
}


//!
/*!  driver_base_t<> operator=

*/
template <SDBG_DEFAULT_TEMPLATE_WIDTH> 
driver_base_t<SDBG_DEFAULT_TEMPLPARAM> & 
driver_base_t<SDBG_DEFAULT_TEMPLPARAM>::operator=
( const driver_base_t &rhs )
{
  evman(NULL);
  lmon(NULL);
  MODULENAME = rhs.MODULENAME;

  return *this;
}


////////////////////////////////////////////////////////////////////
//
// PRIVATE copy constructor and operator= (class rm_driver_base_t)
//
////////////////////////////////////////////////////////////////////


//!
/*!  rm_driver_base_t<> constructors

*/
rm_driver_base_t::rm_driver_base_t (const rm_driver_base_t &d) 
{
  // this copy construct doesn't copy evman and lmon
  rm_evman = d.rm_evman;
  rm_lmon = d.rm_lmon;
  MODULENAME = d.MODULENAME;
}


//!
/*!  rm_driver_base_t<> constructors

*/
rm_driver_base_t &
rm_driver_base_t::operator=(const rm_driver_base_t &rhs) 
{
  // this copy construct doesn't copy evman and lmon
  rm_evman = rhs.rm_evman;
  rm_lmon = rhs.rm_lmon;
  MODULENAME = rhs.MODULENAME;

  return *this;
}


////////////////////////////////////////////////////////////////////
//
// PUBLIC INTERFACES (class driver_base_t<>)
//
////////////////////////////////////////////////////////////////////


//!
/*!  driver_base_t<> constructors

*/
template <SDBG_DEFAULT_TEMPLATE_WIDTH> 
driver_base_t<SDBG_DEFAULT_TEMPLPARAM>::driver_base_t()
  : evman(NULL), lmon(NULL)
{
  //
  // sets the module name for debugging support
  //
  MODULENAME = self_trace_t::driver_module_trace.module_name;
}


//!
/*!  driver_base_t<> dtor

*/
template <SDBG_DEFAULT_TEMPLATE_WIDTH> 
driver_base_t<SDBG_DEFAULT_TEMPLPARAM>::~driver_base_t()
{
  if (evman) 
    {
      //
      // This must be the only place to delete evman 
      //
      delete evman;
    }
  
  if (lmon) 
    {
      //
      // This must be the only place to delete lmon
      // and lmon must have virtual dtor.
      //
      delete lmon;
    }
}

//!
/*!  driver_base_t<> accessors
      
    
*/
template <SDBG_DEFAULT_TEMPLATE_WIDTH> 
event_manager_t<SDBG_DEFAULT_TEMPLPARAM> * 
driver_base_t<SDBG_DEFAULT_TEMPLPARAM>::get_evman()
{ 
  return evman; 
}

template <SDBG_DEFAULT_TEMPLATE_WIDTH> 
launchmon_base_t<SDBG_DEFAULT_TEMPLPARAM> * 
driver_base_t<SDBG_DEFAULT_TEMPLPARAM>::get_lmon()
{ 
  return lmon;  
}

template <SDBG_DEFAULT_TEMPLATE_WIDTH> 
void 
driver_base_t<SDBG_DEFAULT_TEMPLPARAM>::set_evman 
(event_manager_t<SDBG_DEFAULT_TEMPLPARAM> *em) 
{ 
  evman = em; 
} 

template <SDBG_DEFAULT_TEMPLATE_WIDTH> 
void 
driver_base_t<SDBG_DEFAULT_TEMPLPARAM>::set_lmon 
(launchmon_base_t<SDBG_DEFAULT_TEMPLPARAM> *lm) 
{ 
  lmon = lm; 
}

//! drive_engine:
/*! driver_base_t<> drive_engine
    Method that performs the core of event driving
*/
template <SDBG_DEFAULT_TEMPLATE_WIDTH> 
driver_error_e 
driver_base_t<SDBG_DEFAULT_TEMPLPARAM>::drive_engine(opts_args_t *opt)
{
  try 
    {
      using namespace std;

      pid_t pid;

      if ( !evman || !lmon ) 	   
        return SDBG_DRIVER_FAILED;
  
      //
      // launchmon engine initialization
      // this includes connecting to the FE client.
      //
      if (lmon->init( opt ) != LAUNCHMON_OK) 
        return SDBG_DRIVER_FAILED;

      if ( !opt->get_my_opt()->attach ) 
	{
	  //
	  //  
	  //  launch case (I need to create a new process and get that pid) 
	  // 
	  //
	  if ( (pid = fork()) == 0 ) 
	    { 
              //
              // I'm a new RM launcher process and thus the child!
              // 
	      lmon->get_tracer()->tracer_trace_me(); 

              //
	      // Thus, I'm executing the debugtarget which should
              // be the name of RM launcher.
              //
	      execv ( opt->get_my_opt()->debugtarget.c_str(), 
		      opt->get_my_opt()->remaining );
              //                                           //
              //           ** SINK **                      //
              //                                           //
	    }
	}
      else 	
	{
	  //
	  // 
	  //  attach case (all I need to know is the given target pid)
	  // 
	  //
	  pid = opt->get_my_opt()->launcher_pid;   
	}


      {
	self_trace_t::trace ( LEVELCHK(level2), 
	  MODULENAME,
	  0,
	  "now creating a process object based on pid=%d, exe=%s",
	  pid,
	  opt->get_my_opt()->debugtarget.c_str() );
      }
	  

      //
      // create a main process object using create_process(int, string) 
      //
      process_base_t<SDBG_DEFAULT_TEMPLPARAM> *launcher_proc 
        = create_process ( pid, string ( opt->get_my_opt()->debugtarget) );

      //
      // Construct launch string
      //
#ifdef RM_BE_STUB_CMD
      char *pref;
      std::string bestub(RM_BE_STUB_CMD);
      if (pref = getenv("LMON_PREFIX"))
        bestub = std::string(pref) + std::string("/bin/") + bestub;
#endif

      std::string bulklauncher = opt->get_my_opt()->debugtarget;

#ifdef RM_FE_COLOC_CMD
      char *bnbuf = strdup(bulklauncher.c_str());
      char *dt = basename(bnbuf);

      char *pref2;
      bulklauncher = RM_FE_COLOC_CMD;
      if (pref2 = getenv("LMON_PREFIX"))
        {
           bulklauncher = std::string(pref)
                         + std::string("/bin/")
                         + bulklauncher;
        }
#endif
      rc_rm_plat_matcher<VA, EXECHANDLER> platobj; 
      bool initc = platobj.init_rm_instance(*(opt->get_my_rmconfig()),
                            bulklauncher,
                            opt->get_my_opt()->tool_daemon,
                            opt->get_my_opt()->tool_daemon_opts,
			    opt->get_my_opt()->febeconn_info,
			    opt->get_my_opt()->femwconn_info,
                            launcher_proc->get_myimage(),
                            launcher_proc->get_myrmso_image()
#ifdef RM_BE_STUB_CMD
                           ,bestub
#endif
                            );      

      if (!initc)
      {
	self_trace_t::trace ( true, 
	  MODULENAME,
	  0,
	  "Unknown resource manager type: it could be misconfiguration in a rm config file.");
         return SDBG_DRIVER_FAILED;
      }

      //
      // process object carries "opt" around for future reference.
      //
      launcher_proc->set_myopts(opt); 

      //
      // install signal handlers (This is required to beef up the 
      // interactions between this and the parallel launcher proc
      //
      signal_handler_t<SDBG_DEFAULT_TEMPLPARAM> sh;
      sh.set_tracer ( lmon->get_tracer() );
      sh.set_launcher_proc ( launcher_proc );
      sh.set_evman ( evman );
      sh.set_lmon ( lmon );
      sh.install_hdlr_for_all_sigs ( );

      //
      // Attach case needs one more launchmon handler call
      //
      if ( opt->get_my_opt()->attach )
	lmon->handle_attach_event ( (*launcher_proc) );

      //
      // event manager begin monitoring events coming from launcher_proc
      // and the channel connecting to the FE client.
      //
      while ( evman->multiplex_events ( *launcher_proc, *lmon) )
	{

	  //                                              //
	  // * * * * * Main Event Handler Loop * * * * *  //
	  //                                              //
	  //      +                               +       // 
	  //        -----------------------------         //
	}

      //
      // this must be the only place to delete launcher_proc
      // launcher_proc must have a virtual dtor
      //
      delete launcher_proc;

      return SDBG_DRIVER_OK;
    }
  catch ( symtab_exception_t e ) 
    {
      e.report();
      return SDBG_DRIVER_FAILED;
    }
  catch ( tracer_exception_t e ) 
    {
      e.report();
      return SDBG_DRIVER_FAILED;
    } 
  catch ( machine_exception_t e )
    {
      e.report();
      return SDBG_DRIVER_FAILED;
    }
}


//! drive:
/*! driver_base_t<> drive
    The entry point of this method is the standalone case    
*/
template <SDBG_DEFAULT_TEMPLATE_WIDTH> 
driver_error_e 
driver_base_t<SDBG_DEFAULT_TEMPLPARAM>::drive 
( int argc, char **argv )
{
 
  opts_args_t *opt = new opts_args_t();
  driver_error_e rc;

  //
  // processing the commandline options and arguments
  //
  opt->process_args(&argc, &argv);
  rc = drive_engine(opt);
  delete opt;
  opt = NULL;
  return (rc);
}


//! drive:
/*! driver_base_t<> drive
    The entry point of this method is LMON FE APIs case     
*/
template <SDBG_DEFAULT_TEMPLATE_WIDTH> 
driver_error_e 
driver_base_t<SDBG_DEFAULT_TEMPLPARAM>::drive 
( opts_args_t *opt )
{
  return (drive_engine(opt));
}


////////////////////////////////////////////////////////////////////
//
// PUBLIC INTERFACES (class rm_driver_base_t)
//
////////////////////////////////////////////////////////////////////


//! ctor: rm_driver_base_t 
/*! 
 
*/
rm_driver_base_t::rm_driver_base_t ()
{
  rm_evman = NULL;
  rm_lmon = NULL; 
  MODULENAME = self_trace_t::rm_driver_module_trace.module_name; 
}


//! dtor: rm_driver_base_t 
/*! 
 
*/
rm_driver_base_t::~rm_driver_base_t ()
{
  if (rm_evman) {
    delete rm_evman;
  }

  if (rm_lmon) {
    delete rm_lmon;
  }
}


//! accessor: get_rm_evman 
/*! 
 
*/
rm_event_manager_t *
rm_driver_base_t::get_rm_evman ()
{
  return rm_evman;
}


//! accessor: set_rm_evman 
/*! 
 
*/
void 
rm_driver_base_t::set_rm_evman (rm_event_manager_t * em)
{
  rm_evman = em;
}


//! accessor: get_rm_lmon
/*! 
 
*/
rm_api_launchmon_base_t *
rm_driver_base_t::get_rm_lmon ()
{
  return rm_lmon;
}


//! accessor: set_rm_lmon
/*! 
 
*/
void 
rm_driver_base_t::set_rm_lmon (rm_api_launchmon_base_t * lm)
{
  rm_lmon = lm;
}


//! drive 
/*! 
 
*/
driver_error_e
rm_driver_base_t::drive ( opts_args_t *opt )
{
  return (drive_engine(opt));
}


//! drive:
/*! rm_driver_base_t<> drive
    The entry point of this method is the standalone case
 */
driver_error_e
rm_driver_base_t::drive
( int argc, char **argv )
{

  opts_args_t *opt = new opts_args_t();
  driver_error_e rc;

  //
  // processing the commandline options and arguments
  //
  opt->process_args(&argc, &argv);
  rc = drive_engine(opt);
  delete opt;
  opt = NULL;
  return (rc);
}


//! drive_engine 
/*! 
 
*/
driver_error_e
rm_driver_base_t::drive_engine ( opts_args_t *opt )
{
  if ( !rm_evman || !rm_lmon )
    return SDBG_DRIVER_FAILED;

  //
  // RM launchmon engine initialization
  // this includes connecting to the FE client and 
  // initialization of the target RM system.
  //
  if (rm_lmon->init( opt ) != RMAPI_LMON_OK)
        return SDBG_DRIVER_FAILED;
   
  rmapi_signal_handler_t sh;
  sh.set_evman ( rm_evman );
  sh.set_lmon ( rm_lmon );
  sh.set_mask ( );
  sh.install_hdlr_for_all_sigs ( );
  
  //
  // rm_event manager begin monitoring RM events and FE
  // socket events using the rm_lmon object. 
  //
  while ( rm_evman->multiplex_events (*rm_lmon) )
    {
      //                                              //
      // * * * * * Main Event Handler Loop * * * * *  //
      //    for RM launchmon engine                   //
      //      +                               +       //
      //        -----------------------------         //
    }

  return SDBG_DRIVER_OK;
}

#endif // SDBG_BASE_DRIVER_IMPL_HXX
