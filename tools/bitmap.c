/*======================================================================
 *  Copyright 2017 Ivan Augé
 *
 *  This file is part of the YFIG software.
 * 
 *  YFIG is free  software;  you  can redistribute it and/or modify it
 *  under the terms of the GNU  General Public License as published by
 *  the  Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  YFIG software is distributed  in the  hope that it will be useful,
 *  but WITHOUT ANY WARRANTY;  without even the  implied  warranty  of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  the GNU
 *  General Public License for more details.
 * 
 *  You should have received  a copy of the GNU General Public License
 *  along with YFIG. If not, see <http://www.gnu.org/licenses/>.
**======================================================================
 * $Software: YFIG-TOOL (utilities shared by the different modules)
 *      $Id: bitmap.c 123 2018-04-11 08:28:17Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/bitmap.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= See the bitmap-types.h and bitmap.h headers for documentation.     =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/bitmap.h"

TpixmapEGS PXMEGS_none = { .ulong = PXMEGS_NONE };

/*======================================================================*/
/*= Interface to Trawdata (not the right place)                        =*/

static Trawdata* rdt_new(int bs)
{ return tool_malloc( sizeof(Trawdata)+bs ); }

Trawdata* rdt_newBitEated ( int bs, Tuchar* bits)
{
    Trawdata* rdt = rdt_new( bs );
    rdt->refcnt = 1;
    rdt->bytesize = bs;
    memcpy(rdt->data,bits,bs);
    free( bits );
    return rdt;
}

Trawdata* rdt_newBitClone ( int bs, Cuchar* bits);
Trawdata* rdt_newBitStatic( int bs, Cuchar* bits);

Trawdata* rdt_newClone( Crawdata* _rdt )
{
    Trawdata* rdt = rdt_new( _rdt->bytesize );
    rdt->refcnt = 1;
    rdt->bytesize = _rdt->bytesize;
    memcpy(rdt->data,_rdt->data,_rdt->bytesize);
    return rdt;
}

Trawdata* rdt_newLazy ( Crawdata* _rdt )
{
    Trawdata* rdt = (Trawdata*)_rdt;
    if (rdt->refcnt>0) rdt->refcnt += 1;
    return rdt;
}

void      rdt_delete  ( Trawdata* rdt )
{
    if ( rdt->refcnt > 0 ) {
        rdt->refcnt -= 1;
        if ( rdt->refcnt == 0 ) {
            free( rdt );
        }
    }
}

/*======================================================================*/
/*= Interface to Tbitmap                                               =*/

extern Tbitmap* btm_newBitEated (int dx, int dy, Tuchar* bits)
{
    Tbitmap* btm = tool_malloc( sizeof(*btm) );
    btm->dx     = dx;
    btm->dy     = dy;
    btm->bits   = rdt_newBitEated( BTM_ByteSize(dx,dy), bits);
    btm->refcnt = 1;
    return btm;
}

extern Tbitmap* btm_newBitClone (int dx, int dy, Cuchar* bits);
extern Tbitmap* btm_newBitStatic(int dx, int dy, Cuchar* bits);

extern Tbitmap* btm_newClone (Cbitmap* _btm)
{
    Tbitmap* btm = tool_malloc( sizeof(*btm) );
    btm->dx     = _btm->dx;
    btm->dy     = _btm->dy;
    btm->bits   = rdt_newClone( _btm->bits);
    btm->refcnt = 1;
    return btm;
}

extern Tbitmap* btm_newLazy  (Cbitmap* _btm)
{
    Tbitmap* btm = (Tbitmap*)_btm;
    //btm->bits  = rdt_newLazy( _btm->bits );
    if ( btm->refcnt > 0 ) btm->refcnt+=1;
    return btm;
}

extern void     btm_delete   (Tbitmap* btm)
{
    if ( btm->refcnt > 0 ) {
        btm->refcnt -= 1;
        if ( btm->refcnt == 0 ) {
            rdt_delete( btm->bits );
            free( btm );
        }
    }
}

/*======================================================================*/
/*= Interface to TbitmapSet                                            =*/

extern TbitmapSet* btmset_new      (int n)
{
    TbitmapSet* ret =  tool_malloc(sizeof(*ret));
    ret->eleMemNb  = n>=0 ? n : 0; 
    ret->eleNb     = 0;
    ret->eles      = n>0  ?  tool_malloc(sizeof(*ret->eles)*n) : 0;
    return ret;
}

extern TbitmapSet* btmset_newClone (CbitmapSet* s);
extern TbitmapSet* btmset_newLazy  (CbitmapSet* s);
extern void        btmset_delete   (TbitmapSet* s)
{
    if ( s->eles ) {
        int i;
        for (i=0 ; i<s->eleNb ; i+=1)
            btm_delete( s->eles[i] );
        free(s->eles);
    }
    free(s);
}

extern void btmset_addEated (TbitmapSet* s, Tbitmap*btm );
extern void btmset_addClone (TbitmapSet* s, Cbitmap*btm );
extern void btmset_addLazy  (TbitmapSet* s, Tbitmap*btm )
{
    if ( s->eleNb == s->eleMemNb ) {
        s->eleMemNb += 4;
        s->eles = tool_realloc(s->eles,sizeof(*s->eles)*s->eleMemNb);
    }
    s->eles[s->eleNb] = btm_newLazy( btm );
    s->eleNb += 1;
}

/*======================================================================*/
/*= Some conversion                                                    =*/

extern Trawdata* btm_XBM2BitSeq(Cbitmap*btm)
{
    int isrc,ides,x,y;
    Cuchar*   src  = btm->bits->data;
    int       dx   = btm->dx;
    int       dy   = btm->dy;
    Trawdata* ret  = rdt_new( (dx*dy+7)/8 );
              ret->refcnt   = 1;
              ret->bytesize = (dx*dy+7)/8;
    Tuchar*   des  = ret ->data;

    Tuint remaining_bits  =0;
    int   remaining_nbbits=0;
    for (isrc=0,ides=0,y=0 ; y<dy ; y+=1 ) {
      for ( x=0 ; x<dx ; x+=8,isrc+=1 ) {
//printf("XXX:%s:0: dx=%d dy=%d isrc=%2d ides=%d remain=%04x (%d)\n",
//        __func__,dx,dy,isrc,ides,remaining_bits,remaining_nbbits);
          Tuint src_bits   = src[isrc];
          int   src_nbbits = 8;
          if ( (x+8)>dx ) {
              // eg: dx=21 & x=16
              // xxxxxxxx xxxxxxxx yyyyyuuu 
              // --> src_nbbits=5 & src_bits=000yyyyy
              src_nbbits = dx - x;
              //src_bits   = src_bits >> (8-src_nbbits);
              src_bits   = src_bits & ((1<<src_nbbits)-1);
          }
//printf("  X:%s:1: dx=%d dy=%d isrc=%2d ides=%d remain=%04x (%d) src=%04x (%d)\n",
//        __func__,dx,dy,isrc,ides,remaining_bits,remaining_nbbits,src_bits,src_nbbits);
          // now we concat the remaining bits with the new
          // src bits into the bits variable.
          // remaining_bits: xxxrrrrr
          // src_bits:       xxxxssss
          // --> bits:  ...xrrrrrssss
          Tuint bits=0;
          int   nbbits=0;
          if ( remaining_nbbits ) {
              bits   = remaining_bits;
              nbbits = remaining_nbbits;
          }
          if ( src_nbbits ) {
              bits    = bits<<src_nbbits | src_bits;
              nbbits += src_nbbits;
          }
////printf("  X:%s:2: dx=%d dy=%d isrc=%2d ides=%d remain=%04x (%d) bit=%04x (%d)\n",
//        __func__,dx,dy,isrc,ides,remaining_bits,remaining_nbbits,bits,nbbits);
          if ( nbbits<8 ) {
              // not enough bit to build a byte, bits become the remaining bits.
              remaining_bits   = bits;
              remaining_nbbits = nbbits;
          } else { // nbbits>=8
              // we write the 8 msb bits in the des bit sequence,
              // the no written bits become the remaining bits.
              remaining_nbbits = nbbits-8;
              Tuchar byte = bits >> remaining_nbbits;
              des[ides++] = byte;
              remaining_bits   = bits & ((1<<remaining_nbbits)-1);
//printf("  X:%s:4: dx=%d dy=%d isrc=%2d ides=%d ((1<<%d)-1)=%04x\n",
//        __func__,dx,dy,isrc,ides,remaining_nbbits,((1<<remaining_nbbits)-1));
//printf("  X:%s:4: dx=%d dy=%d isrc=%2d ides=%d remain=%04x (%d) byt=%04x (%d)\n",
//        __func__,dx,dy,isrc,ides,remaining_bits,remaining_nbbits,byte,nbbits);
          }
      }
    }

    return ret;
}

/*======================================================================*/
/*= Image.                                                             =*/

extern int  image_canonical(int ope)
{
    ope = ope & (IMG_OP_FLIP_H|IMG_OP_FLIP_V|IMG_OP_ROTA);
    switch ( ope ) {
        case (IMG_OP_ROTA_P000)               : return IMG_OP_CAN_FX0FY0R000;
        case (IMG_OP_ROTA_P000|IMG_OP_FLIP_H) : return IMG_OP_CAN_FX1FY0R000;
        case (IMG_OP_ROTA_P000|IMG_OP_FLIP_V) : return IMG_OP_CAN_FX0FY1R000;
        case (IMG_OP_ROTA_P000|IMG_OP_FLIP_HV): return IMG_OP_CAN_FX1FY1R000;
        case (IMG_OP_ROTA_P090)               : return IMG_OP_CAN_FX0FY0R090;
        case (IMG_OP_ROTA_P090|IMG_OP_FLIP_H) : return IMG_OP_CAN_FX1FY0R090;
        case (IMG_OP_ROTA_P090|IMG_OP_FLIP_V) : return IMG_OP_CAN_FX0FY1R090;
        case (IMG_OP_ROTA_P090|IMG_OP_FLIP_HV): return IMG_OP_CAN_FX1FY1R090;
        case (IMG_OP_ROTA_P180)               : return IMG_OP_CAN_FX1FY1R000;
        case (IMG_OP_ROTA_P180|IMG_OP_FLIP_H) : return IMG_OP_CAN_FX0FY1R000;
        case (IMG_OP_ROTA_P180|IMG_OP_FLIP_V) : return IMG_OP_CAN_FX1FY0R000;
        case (IMG_OP_ROTA_P180|IMG_OP_FLIP_HV): return IMG_OP_CAN_FX0FY0R000;
        case (IMG_OP_ROTA_P270)               : return IMG_OP_CAN_FX1FY1R090;
        case (IMG_OP_ROTA_P270|IMG_OP_FLIP_H) : return IMG_OP_CAN_FX0FY1R090;
        case (IMG_OP_ROTA_P270|IMG_OP_FLIP_V) : return IMG_OP_CAN_FX1FY0R090;
        case (IMG_OP_ROTA_P270|IMG_OP_FLIP_HV): return IMG_OP_CAN_FX0FY0R090;
        default:
            PERM_spf_printf("%02x is an invalid image operation.",ope);
            return 0;
    }
}

extern void image_canonical_hvr(int ope, int*fx, int*fy, int*rot)
{
    ope = image_canonical(ope);
    *fx   = (ope&IMG_OP_FLIP_H)!=0 ;
    *fy   = (ope&IMG_OP_FLIP_V)!=0 ;
    *rot  = (ope&IMG_OP_ROTA) ;
}

/*======================================================================*/
