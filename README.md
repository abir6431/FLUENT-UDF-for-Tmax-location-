# FLUENT-UDF-for-Tmax-location-
This UDF find out maximum temperature and its location within given zones. It will also write a *.scm file with the Z-coordinate and the TUI commands to make a 2D plane at that Z location. 

The UDF is based on the following algorithm:
1.	Find maximum temperature (T) within the given solid zones. 
2.	Find location (Z) of this maximum temperature (T).
3.	Write a file of the form *.scm which has the TUI commands to plot contours of the hottest section at the location (Z).

The *.scm file obtained from running the UDF must be read into the FLUENT input/Journal file. To run the UDF and then produce contours of hottest section add the following commands to the FLUENT input/Journal file.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;unload existing libudf if there is any
/define/user-defined/compiled-functions
unload
"libudf"    ;libudf being the currently loaded UDF 

!rm -r libudf   ; remove the existing libudf folder

;;;compile the hottest section UDF;;;;;;;;;;;
/define/user-defined/compiled-functions
compile
"libudftop"
yes
"hot_sec_udf.c"

;;;load compiled UDF
/define/user-defined/cf/load
"libudftop"

;;Execute_On_Demand
/define/user-defined/execute-on-demand "max_temp_loc_top::libudftop"

;;read the scheme created by UDF
/file/read-macros
pp.scm
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
