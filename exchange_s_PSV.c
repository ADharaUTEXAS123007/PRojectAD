/*------------------------------------------------------------------------
 *   write values of dynamic field variables at the edges of the
 *   local grid into buffer arrays and  exchanged between
 *   processes.
 *  ----------------------------------------------------------------------*/

#include "fd.h"

void exchange_s_PSV(float ** sxx, float ** syy, 
	float ** sxy, float ** bufferlef_to_rig, float ** bufferrig_to_lef, 
	float ** buffertop_to_bot, float ** bufferbot_to_top,
	MPI_Request * req_send, MPI_Request * req_rec){

	extern MPI_Comm SHOT_COMM;
	extern int NX, NY, POS[3], NPROCX, NPROCY, BOUNDARY, FDORDER;
	extern int INDEX[5];
	extern const int TAG1,TAG2,TAG5,TAG6;
	MPI_Status  status;
	int i, j, fdo, fdo3, n, l;


	fdo = FDORDER/2 + 1;
	fdo3 = 2*fdo;
	
	
	/* top - bottom */
	
	if (POS[2]!=0)	/* no boundary exchange at top of global grid */
	for (i=1;i<=NX;i++){
		/* storage of top of local volume into buffer */
		n = 1;
		for (l=1;l<=fdo-1;l++) {
			buffertop_to_bot[i][n++]  = sxy[l][i];
		}
		for (l=1;l<=fdo;l++) {
			buffertop_to_bot[i][n++]  = syy[l][i];
		}
	}


	if (POS[2]!=NPROCY-1)	/* no boundary exchange at bottom of global grid */
	for (i=1;i<=NX;i++){			
		/* storage of bottom of local volume into buffer */
		n = 1;
		for (l=1;l<=fdo;l++) {
			bufferbot_to_top[i][n++]  = sxy[NY-l+1][i];
		}
		for (l=1;l<=fdo-1;l++) {
			bufferbot_to_top[i][n++]  = syy[NY-l+1][i];
		}
	}
	
	
	/* alternative communication */
	/* still blocking communication */
	MPI_Sendrecv_replace(&buffertop_to_bot[1][1],NX*fdo3,MPI_FLOAT,INDEX[3],TAG5,INDEX[4],TAG5,SHOT_COMM,&status);
	MPI_Sendrecv_replace(&bufferbot_to_top[1][1],NX*fdo3,MPI_FLOAT,INDEX[4],TAG6,INDEX[3],TAG6,SHOT_COMM,&status);
	


	if (POS[2]!=NPROCY-1)	/* no boundary exchange at bottom of global grid */
	for (i=1;i<=NX;i++){
		n = 1;
		for (l=1;l<=fdo-1;l++) {
			sxy[NY+l][i] = buffertop_to_bot[i][n++];
		}
		for (l=1;l<=fdo;l++) {
			syy[NY+l][i] = buffertop_to_bot[i][n++];
		}
	}

	if (POS[2]!=0)	/* no boundary exchange at top of global grid */
	for (i=1;i<=NX;i++){
		n = 1;
		for (l=1;l<=fdo;l++) {
			sxy[1-l][i] = bufferbot_to_top[i][n++];
		}
		for (l=1;l<=fdo-1;l++) {
			syy[1-l][i] = bufferbot_to_top[i][n++];
		}
	}
	
	
	/* left - right */

	if ((BOUNDARY) || (POS[1]!=0))	/* no boundary exchange at left edge of global grid */
	for (j=1;j<=NY;j++){
		/* storage of left edge of local volume into buffer */
		n = 1;
		for (l=1;l<fdo-1;l++) {
			bufferlef_to_rig[j][n++] =  sxy[j][l];
		}
		for (l=1;l<fdo;l++) {
			bufferlef_to_rig[j][n++] =  sxx[j][l];
		}
	}


	if ((BOUNDARY) || (POS[1]!=NPROCX-1))	/* no boundary exchange at right edge of global grid */
	for (j=1;j<=NY;j++){
		/* storage of right edge of local volume into buffer */
		n = 1;
		for (l=1;l<fdo;l++) {
			bufferrig_to_lef[j][n++] =  sxy[j][NX-l+1];
		}
		for (l=1;l<fdo-1;l++) {
			bufferrig_to_lef[j][n++] =  sxx[j][NX-l+1];
		}
	}	
	

	
	/* alternative communication */
	/* still blocking communication */
	MPI_Sendrecv_replace(&bufferlef_to_rig[1][1],NY*fdo3,MPI_FLOAT,INDEX[1],TAG1,INDEX[2],TAG1,SHOT_COMM,&status);
	MPI_Sendrecv_replace(&bufferrig_to_lef[1][1],NY*fdo3,MPI_FLOAT,INDEX[2],TAG2,INDEX[1],TAG2,SHOT_COMM,&status);	

	
	if ((BOUNDARY) || (POS[1]!=NPROCX-1))	/* no boundary exchange at right edge of global grid */
	for (j=1;j<=NY;j++){
		n = 1;
		for (l=1;l<fdo-1;l++) {
			sxy[j][NX+l] = bufferlef_to_rig[j][n++];
		}
		for (l=1;l<fdo;l++) {
			sxx[j][NX+l] = bufferlef_to_rig[j][n++];
		}
	}

	if ((BOUNDARY) || (POS[1]!=0))	/* no boundary exchange at left edge of global grid */
	for (j=1;j<=NY;j++){
		n = 1;
		for (l=1;l<fdo;l++) {
			sxy[j][1-l] = bufferrig_to_lef[j][n++];
		}
		for (l=1;l<fdo-1;l++) {
			sxx[j][1-l] = bufferrig_to_lef[j][n++];
		}
	}


}
