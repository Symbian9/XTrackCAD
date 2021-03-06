/** \file misc2.h
 * 
 */

/*  XTrkCad - Model Railroad CAD
 *  Copyright (C) 2005 Dave Bullis
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef MISC2_H
#define MISC2_H

#include "common.h"
#include "misc.h"
#include "time.h"

#define LABEL_MANUF		(1<<0)
#define LABEL_PARTNO	(1<<1)
#define LABEL_DESCR		(1<<2)
#define LABEL_COST		(1<<7)
#define LABEL_FLIPPED	(1<<8)
#define LABEL_TABBED	(1<<9)
#define LABEL_UNGROUPED (1<<10)
#define LABEL_SPLIT		(1<<11)

typedef struct {
		char * name;
		int level;
		} logTable_t;
extern dynArr_t logTable_da;
#define logTable(N) DYNARR_N( logTable_t, logTable_da, N )
time_t logClock;
void LogOpen( char * );
void LogClose( void );
void LogSet( char *, int );
int LogFindIndex( char * );
void LogPrintf( char *, ... );
#define LOG( DBINX, DBLVL, DBMSG ) \
		if ( DBINX > 0 && logTable( DBINX ).level >= DBLVL ) { \
				LogPrintf DBMSG ; \
		}
#define LOG1( DBINX, DBMSG ) LOG( DBINX, 1, DBMSG )
#define LOGNAME( DBNAME, DBMSG ) LOG( LogFindIndex( DBNAME ), DBMSG )

#define lprintf LogPrintf
void Rdump( FILE * );
void Rprintf( char *, ... );

typedef struct {
		DIST_T length;
		DIST_T width;
		DIST_T spacing;
		} tieData_t, *tieData_p;
DIST_T GetScaleTrackGauge( SCALEINX_T );
DIST_T GetScaleRatio( SCALEINX_T );
DIST_T GetScaleDescRatio( SCALEDESCINX_T sdi );
char * GetScaleName( SCALEINX_T );
SCALEINX_T GetScaleInx( SCALEDESCINX_T scaleInx, GAUGEINX_T gaugeInx );

char *GetScaleDesc( SCALEDESCINX_T inx );
char *GetGaugeDesc( SCALEDESCINX_T scaleInx, GAUGEINX_T gaugeInx );
void GetScaleEasementValues( DIST_T *, DIST_T * );
tieData_p GetScaleTieData( SCALEINX_T );
SCALEINX_T LookupScale( const char * );
BOOL_T GetScaleGauge( SCALEINX_T scaleInx, SCALEDESCINX_T *scaleDescInx, GAUGEINX_T *gaugeInx);
void SetScaleGauge(SCALEDESCINX_T desc, GAUGEINX_T gauge);
BOOL_T DoSetScale( char * );

void ScaleLengthIncrement( SCALEINX_T, DIST_T );
void LoadScaleList( wList_p );
void LoadGaugeList( wList_p, SCALEDESCINX_T );
BOOL_T CompatibleScale( BOOL_T, SCALEINX_T, SCALEINX_T );
BOOL_T DoSetScaleDesc( void );

unsigned int curLayer;
long layerCount;
wDrawColor GetLayerColor( unsigned int );
BOOL_T GetLayerVisible( unsigned int );
BOOL_T GetLayerFrozen( unsigned int );
BOOL_T GetLayerOnMap( unsigned int );
char * GetLayerName( unsigned int );
BOOL_T ReadLayers( char * );
BOOL_T WriteLayers( FILE * );
char * FormatLayerName(unsigned int layerNumber);
/* dlayers.c */
void UpdateLayerLists( void );
void DefaultLayerProperties(void);
void ResetLayers( void );
void SaveLayers( void );
void RestoreLayers( void );
void LoadLayerLists( void );
addButtonCallBack_t InitLayersDialog( void );

void Misc2Init( void );

#endif
