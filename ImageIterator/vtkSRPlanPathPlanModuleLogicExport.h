/*=auto=========================================================================

 Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) 
 All Rights Reserved.

 See COPYRIGHT.txt
 or http://www.slicer.org/copyright/copyright.txt for details.

 Program:   SRPlan

=========================================================================auto=*/


// .NAME __vtkSRPlanPathPlanModuleLogicExport - manage Windows system differences
// .SECTION Description
// The __vtkSRPlanPathPlanModuleLogicExport captures some system differences between Unix
// and Windows operating systems. 

#ifndef __vtkSRPlanPathPlanModuleLogicExport_h
#define __vtkSRPlanPathPlanModuleLogicExport_h

#if defined(WIN32) && !defined(vtkSRPlanPathPlanModuleLogic_STATIC)
 #if defined(vtkSRPlanPathPlanModuleLogic_EXPORTS)
  #define VTK_SRPlan_PATHPLAN_MODULE_LOGIC_EXPORT __declspec( dllexport )
 #else
  #define VTK_SRPlan_PATHPLAN_MODULE_LOGIC_EXPORT __declspec( dllimport )
 #endif
#else
 #define VTK_SRPlan_PATHPLAN_MODULE_LOGIC_EXPORT
#endif

#endif
