/**********************************************************************
   UDF to calculate maximum temperature and its location within given zones.
   It will also write a *.scm file with the Z-coordinate and the TUI commands
   to make a 2D plane at that Z location.  
 ***********************************************************************/
/************************************************************************
	Input needed in this program:
	1. zone IDs of the zones for which the maximum temperature is sought.
	    The zone IDs are input here:
	    a. Line 26:{N1,N2,.....,Nn}
	    b. Line 93: 
*************************************************************************/


#include "udf.h"
#include "prf.h" /*header for global reduction operator (FLUENT customization manual V17.1 Section 7.5.4.)*/
/*excludes the HOST PROCESS :: start*/
int myid;
int node_zero = 0;
#define I_AM_NODE_ZERO_P (myid == node_zero)	/*define compute node-0. We want the file to be writtent from compute node-0*/

void loop_over_zones(int IDs[],int IDlen);
 
DEFINE_ON_DEMAND(max_temp_loc_top)
 {
	#if !RP_HOST
  	int zone_ID[]={380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,396,397,398,399,400,401};	/*Lookup_Thread macro will look over the zone IDs provided in this array*/
   	int ID_len=sizeof(zone_ID)/sizeof(int);	/*This finds the length of the zone_ID[] array*/
   	loop_over_zones(zone_ID,ID_len);
	#endif				/*excludes the HOST PROCESS :: end*/
}
     
void loop_over_zones(int zoneID[],int IDlen)
{ 
	
	real tmax = REAL_MIN, tmin=REAL_MAX;
	real tmpmax=-100, tmpmin=5000;   									
	Domain *d=Get_Domain(1);	/*domain is required argument in the Lookup_Thread(d,zone_ID[i]) macro*/						
	int i;
	cell_t c;
	real GLOBALMAX;
	real GLOBALMIN;
	real x[ND_ND];		/*declaration of the x argument in C_CENTROID(x,c,t) FLUENT Macro*/
	real xloc[N]={-1e7};
	real yloc[N]={-1e7};
	real zloc[N]={-1e7};	/*defines x,y,z co-ordinates in x[ND_ND] and initiates them to very low values*/
	Thread *t;
	
	for (i=0;i<IDlen;i++)		/*the for loop will loop over each zone_ID element*/
	{							
      Thread *t=Lookup_Thread(d,zoneID[i]);	/*lookup the thread IDs in zone_ID*/
      begin_c_loop(c,t)		/* Loops over the cell threads */
      {
		if (C_T(c,t) < tmin) tmin = C_T(c,t);	/*finds min temp in each zone*/
		if (C_T(c,t)> tmax) tmax = C_T(c,t);	/*finds max temp in each zone*/
      }
      end_c_loop(c,t)
      if (tmax>tmpmax) tmpmax=tmax;		/* finds temp max in all the zones listed in zone_ID */ 	
	  if (tmin<tmpmin) tmpmin=tmin;		/* finds temp min in all the zones listed in zone_ID */
	}
	GLOBALMAX = PRF_GRHIGH1(tmpmax);	/* global reduction operator to collect data from all compute nodes to a single value. this one for	max temp */	
	GLOBALMIN = PRF_GRLOW1 (tmpmin);	/* this for min */
	

	thread_loop_c(t,d)
    {
		begin_c_loop(c,t)
        {
  			C_CENTROID(x,c,t);
  			if (C_T(c,t)==GLOBALMAX)
				{
				
				xloc[N] = x[0];
				yloc[N] = x[1];
				zloc [N]= x[2];
				Message ("\n max = %.2f : min = %.2f",GLOBALMAX,GLOBALMIN);
				Message("\n x-value = %.5f  y-value = %.5f  z-value = %.5f",xloc[N],yloc[N],zloc[N]);
				}
		}
		end_c_loop(c,t)
	}
	yloc[N] = PRF_GRHIGH1(yloc[N]);
	xloc[N] = PRF_GRHIGH1(xloc[N]);
	real z1loc = PRF_GRHIGH1(zloc[N]); 
	if (z1loc==0)
		{z1loc=PRF_GRLOW1(zloc[N]);}

	
	if I_AM_NODE_ZERO_P
	{			
	FILE *fp;		/*declare the file where scheme commands are to be written*/
	fp=fopen("./TUI_command_with_max_t_loc-top.scm","w");
	fprintf(fp, "(Ti-menu-load-string (format #f \"surface/iso-surface z-coordinate hottest-section-plane-top () 380	381	382	383	384	385	386	387	388	389	390	391	392	393	394	395	396	397	398	399	400	401 () %.5f\" ))", z1loc);
	fclose(fp);
	}
	}










