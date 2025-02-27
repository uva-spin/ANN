#ifndef TVA1_UU_H
#define TVA1_UU_H

#include <math.h>
#include "LorentzVector.h"

typedef struct {
    double ALP_INV; // 1 / Electromagnetic Fine Structure Constant
    double PI;
    double RAD;
    double M; //Mass of the proton in GeV
    double GeV2nb; // Conversion from GeV to NanoBarn

    double QQ, x, t, k;
    double y, e, xi, tmin, kpr, gg, q, qp, po, pmag, cth, theta, sth, sthl, cthl, cthpr, sthpr, M2, tau;
    double s;     // Mandelstam variable s which is the center of mass energy
    double Gamma; // Factor in front of the cross section
    double jcob;  //Defurne's Jacobian

    // 4-momentum vectors
    LorentzVector K, KP, Q, QP, D, p, P;
    // 4 - vector products independent of phi
    double kkp, kq, kp, kpp;
    // 4 - vector products dependent of phi
    double kd, kpd, kP, kpP, kqp, kpqp, dd, Pq, Pqp, qd, qpd;
    // Transverse 4-vector products
    double kk_T, kqp_T, kkp_T, kd_T, dd_T, kpqp_T, kP_T, kpP_T, qpP_T, kpd_T, qpd_T;
    //Expressions that appear in the polarized interference coefficient calculations
    double Dplus, Dminus;

    double FUUT;
    double AUUBH, BUUBH; // Coefficients of the BH unpolarized structure function FUU_BH
    double AUUI, BUUI, CUUI; // Coefficients of the BHDVCS interference unpolarized structure function FUU_I
    double con_AUUBH, con_BUUBH; // Coefficients times the conversion to nb and the jacobian
    double con_AUUI, con_BUUI, con_CUUI;  // Coefficients times the conversion to nb and the jacobian
    double bhAUU, bhBUU; // Auu and Buu term of the BH cross section
    double iAUU, iBUU, iCUU; // Terms of the interference containing AUUI, BUUI and CUUI
    double xdvcsUU, xbhUU, xIUU; // Unpolarized cross sections
} TVA1_UU;



void TVA1_UU_Init(TVA1_UU *self);
double TVA1_UU_TProduct(LorentzVector v1, LorentzVector v2);
void TVA1_UU_SetKinematics(TVA1_UU *self, double _QQ, double _x, double _t, double _k);
void TVA1_UU_Set4VectorsPhiDep(TVA1_UU *self, double phi);
void TVA1_UU_Set4VectorProducts(TVA1_UU *self);
double TVA1_UU_GetDVCSUU(TVA1_UU *self, double ReH, double ReE, double ReHtilde, double ReEtilde, double ImH, double ImE, double ImHtilde, double ImEtilde);
double TVA1_UU_GetBHUU(TVA1_UU *self, double phi, double F1, double F2);
double TVA1_UU_GetIUU(TVA1_UU *self, double phi, double F1, double F2, double ReH, double ReE, double ReHtilde);


void TVA1_UU_Init(TVA1_UU *self) {
    self->ALP_INV = 137.0359998; // 1 / Electromagnetic Fine Structure Constant
    self->PI = 3.1415926535;
    self->RAD = self->PI / 180.;
    self->M = 0.938272; //Mass of the proton in GeV
    self->GeV2nb = .389379*1000000; // Conversion from GeV to NanoBarn
}


double TVA1_UU_TProduct(LorentzVector v1, LorentzVector v2) {
    // Transverse product (tv1v2)
    return v1.x * v2.x + v1.y * v2.y;
}



void TVA1_UU_SetKinematics(TVA1_UU * self, double _QQ, double _x, double _t, double _k){

    self->QQ = _QQ; //Q^2 value
    self->x = _x;   //Bjorken x
    self->t = _t;   //momentum transfer squared
    self->k = _k;   //Electron Beam Energy
    self->M2 = self->M*self->M; //Mass of the proton  squared in GeV
    //fractional energy of virtual photon
    self->y = self->QQ / ( 2. * self->M * self->k * self->x ); // From eq. (23) where gamma is substituted from eq (12c)
    //squared gamma variable ratio of virtuality to energy of virtual photon
    self->gg = 4. * self->M2 * self->x * self->x / self->QQ; // This is gamma^2 [from eq. (12c)]
    //ratio of longitudinal to transverse virtual photon flux
    self->e = ( 1 - self->y - ( self->y * self->y * (self->gg / 4.) ) ) / ( 1. - self->y + (self->y * self->y / 2.) + ( self->y * self->y * (self->gg / 4.) ) ); // epsilon eq. (32)
    //Skewness parameter
    self->xi = 1. * self->x * ( ( 1. + self->t / ( 2. * self->QQ ) ) / ( 2. - self->x + self->x * self->t / self->QQ ) ); // skewness parameter eq. (12b) note: there is a minus sign on the write up that shouldn't be there
    //xi = x * ( ( 1. ) / ( 2. - x ) );
    //Minimum t value
    self->tmin = -( self->QQ * ( 1. - sqrt( 1. + self->gg ) + self->gg / 2. ) ) / ( self->x * ( 1. - sqrt( 1. + self->gg ) + self->gg / ( 2.* self->x ) ) ); // minimum t eq. (29)
    //Final Lepton energy
    self->kpr = self->k * ( 1. - self->y ); // k' from eq. (23)
    //outgoing photon energy
    self->qp = self->t / 2. / self->M + self->k - self->kpr; //q' from eq. bellow to eq. (25) that has no numbering. Here nu = k - k' = k * y
    //Final proton Energy
    self->po = self->M - self->t / 2. / self->M; // This is p'_0 from eq. (28b)
    self->pmag = sqrt( ( -self->t ) * ( 1. - self->t / 4. / self->M / self->M ) ); // p' magnitude from eq. (28b)
    //Angular Kinematics of outgoing photon
    self->cth = -1. / sqrt( 1. + self->gg ) * ( 1. + self->gg / 2. * ( 1. + self->t / self->QQ ) / ( 1. + self->x * self->t / self->QQ ) ); // This is cos(theta) eq. (26)
    self->theta = acos(self->cth); // theta angle
    //Lepton Angle Kinematics of initial lepton
    self->sthl = sqrt( self->gg ) / sqrt( 1. + self->gg ) * ( sqrt ( 1. - self->y - self->y * self->y * self->gg / 4. ) ); // sin(theta_l) from eq. (22a)
    self->cthl = -1. / sqrt( 1. + self->gg ) * ( 1. + self->y * self->gg / 2. ) ; // cos(theta_l) from eq. (22a)
    //ratio of momentum transfer to proton mass
    self->tau = -0.25 * self->t / self->M2;

    // phi independent 4 - momenta vectors defined on eq. (21) -------------
    self->K = LorentzVector_SetPxPyPzE( self->k * self->sthl, 0.0, self->k * self->cthl, self->k );
    self->KP = LorentzVector_SetPxPyPzE( self->K.x, 0.0, self->k * ( self->cthl + self->y * sqrt( 1. + self->gg ) ), self->kpr );
    self->Q = LorentzVector_sub(self->K, self->KP);
    self->p = LorentzVector_SetPxPyPzE(0.0, 0.0, 0.0, self->M);

    // Sets the Mandelstam variable s which is the center of mass energy
    self->s = LorentzVector_mul(LorentzVector_add(self->p, self->K), LorentzVector_add(self->p, self->K));

    // The Gamma factor in front of the cross section
    self->Gamma = 1. / self->ALP_INV / self->ALP_INV / self->ALP_INV / self->PI / self->PI / 16. / ( self->s - self->M2 ) / ( self->s - self->M2 ) / sqrt( 1. + self->gg ) / self->x;

    // Defurne's Jacobian
    self->jcob = 2. * self->PI;
}



void TVA1_UU_Set4VectorsPhiDep(TVA1_UU * self, double phi) {
  // phi dependent 4 - momenta vectors defined on eq. (21) -------------
    self->QP = LorentzVector_SetPxPyPzE(self->qp * sin(self->theta) * cos( phi * self->RAD ), self->qp * sin(self->theta) * sin( phi * self->RAD ), self->qp * cos(self->theta), self->qp);
    self->D = LorentzVector_sub(self->Q, self->QP); // delta vector eq. (12a)
    LorentzVector pp = LorentzVector_add(self->p, self->D); // p' from eq. (21)
    self->P = LorentzVector_add(self->p, pp);
    self->P = LorentzVector_SetPxPyPzE(.5 * self->P.x, .5 * self->P.y, .5 * self->P.z, .5 * self->P.t);
}



void TVA1_UU_Set4VectorProducts(TVA1_UU * self) {
  // 4-vectors products (phi - independent)
  self->kkp  = LorentzVector_mul(self->K, self->KP);   //(kk')
  self->kq   = LorentzVector_mul(self->K,  self->Q);    //(kq)
  self->kp   = LorentzVector_mul(self->K, self->p);    //(pk)
  self->kpp  = LorentzVector_mul(self->KP, self->p);   //(pk')

  // 4-vectors products (phi - dependent)
  self->kd   = LorentzVector_mul(self->K, self->D);    //(kΔ)
  self->kpd  = LorentzVector_mul(self->KP, self->D);   //(k'Δ)
  self->kP   = LorentzVector_mul(self->K, self->P);    //(kP)
  self->kpP  = LorentzVector_mul(self->KP, self->P);   //(k'P)
  self->kqp  = LorentzVector_mul(self->K, self->QP);   //(kq')
  self->kpqp = LorentzVector_mul(self->KP, self->QP);  //(k'q')
  self->dd   = LorentzVector_mul(self->D, self->D);    //(ΔΔ)
  self->Pq   = LorentzVector_mul(self->P, self->Q);    //(Pq)
  self->Pqp  = LorentzVector_mul(self->P, self->QP);   //(Pq')
  self->qd   = LorentzVector_mul(self->Q, self->D);    //(q
  self->qpd  = LorentzVector_mul(self->QP, self->D);   //(q'Δ)

  // Transverse vector products defined after eq.(241c) -----------------------
  self->kk_T   = TVA1_UU_TProduct(self->K, self->K);
  self->kkp_T  = self->kk_T;
  self->kqp_T  = TVA1_UU_TProduct(self->K, self->QP);
  self->kd_T   = -1.* self->kqp_T;
  self->dd_T   = TVA1_UU_TProduct(self->D, self->D);
  self->kpqp_T = self->kqp_T;
  self->kP_T   = TVA1_UU_TProduct(self->K, self->P);
  self->kpP_T  = TVA1_UU_TProduct(self->KP, self->P);
  self->qpP_T  = TVA1_UU_TProduct(self->QP, self->P);
  self->kpd_T  = -1.* self->kqp_T;
  self->qpd_T  = -1. * self->dd_T;

  //Expressions that appear in the interference coefficient calculations
  self->Dplus   = .5 / self->kpqp - .5 / self->kqp;
  self->Dminus  = -.5 / self->kpqp - .5 / self->kqp;
}



//====================================================================================
// DVCS Unpolarized Cross Section
//====================================================================================
double TVA1_UU_GetDVCSUU(TVA1_UU * self, double ReH, double ReE, double ReHtilde, double ReEtilde, double ImH, double ImE, double ImHtilde, double ImEtilde) {
  // Note: SetKinematics should have been previously called.
  self->FUUT = 4.* ( ( 1 - self->xi * self->xi ) * ( ReH * ReH + ImH * ImH + ReHtilde * ReHtilde + ImHtilde * ImHtilde ) + ( self->tmin - self->t ) / ( 2.* self->M2 ) * ( ReE * ReE + ImE * ImE + self->xi * self->xi * ReEtilde * ReEtilde + self->xi * self->xi * ImEtilde * ImEtilde )
         - ( 2.* self->xi * self->xi ) / ( 1 - self->xi * self->xi ) * ( ReH * ReE + ImH * ImE + ReHtilde * ReEtilde + ImHtilde * ImEtilde ) );

  self->xdvcsUU =  self->GeV2nb * self->jcob * self->Gamma / self->QQ / ( 1 - self->e ) * self->FUUT;

  return self->xdvcsUU;
}



//====================================================================================
// BH Unpolarized Cross Section
//====================================================================================
double TVA1_UU_GetBHUU(TVA1_UU * self, double phi, double F1, double F2) {

  // Get the 4-vector products. Note: SetKinematics should have been previously called.
  TVA1_UU_Set4VectorsPhiDep(self, phi);
  TVA1_UU_Set4VectorProducts(self);

  // Coefficients of the BH unpolarized structure function FUU_BH
  self->AUUBH = (8. * self->M2) / (self->t * self->kqp * self->kpqp) * ( (4. * self->tau * (self->kP * self->kP + self->kpP * self->kpP) ) - ( (self->tau + 1.) * (self->kd * self->kd + self->kpd * self->kpd) ) );  //eq. 147
  self->BUUBH = (16. * self->M2) / (self->t * self->kqp * self->kpqp) * (self->kd * self->kd + self->kpd * self->kpd); // eq. 148

  // Convert Unpolarized Coefficients to nano-barn and use Defurne's Jacobian
  self->con_AUUBH = self->AUUBH * self->GeV2nb * self->jcob;
  self->con_BUUBH = self->BUUBH * self->GeV2nb * self->jcob;

  // Unpolarized Coefficients multiplied by the Form Factors
  self->bhAUU = (self->Gamma/self->t) * self->con_AUUBH * ( F1 * F1 + self->tau * F2 * F2 );
  self->bhBUU = (self->Gamma/self->t) * self->con_BUUBH * ( self->tau * ( F1 + F2 ) * ( F1 + F2 ) );

  // Unpolarized BH cross section
  self->xbhUU = self->bhAUU + self->bhBUU;

  return self->xbhUU;
}



//====================================================================================
// Unpolarized BH-DVCS Interference Cross Section (Comparison paper)
//====================================================================================
double TVA1_UU_GetIUU(TVA1_UU *self, double phi, double F1, double F2, double ReH, double ReE, double ReHtilde) {

  // Get the 4-vector products. Note: SetKinematics should have been previously called.
  TVA1_UU_Set4VectorsPhiDep(self, phi);
  TVA1_UU_Set4VectorProducts(self);

  self->AUUI = -4. * cos( phi * self->RAD ) * ( self->Dplus * ( ( self->kqp_T - 2. * self->kk_T - 2. * self->kqp ) * self->kpP + ( 2. * self->kpqp - 2. * self->kkp_T - self->kpqp_T ) * self->kP + self->kpqp * self->kP_T + self->kqp * self->kpP_T - 2.*self->kkp * self->kP_T ) -
                  self->Dminus * ( ( 2. * self->kkp - self->kpqp_T - self->kkp_T ) * self->Pqp + 2. * self->kkp * self->qpP_T - self->kpqp * self->kP_T - self->kqp * self->kpP_T ) ) ;

  self->BUUI = -2. * self->xi * cos( phi * self->RAD ) * ( self->Dplus * ( ( self->kqp_T - 2. * self->kk_T - 2. * self->kqp ) * self->kpd + ( 2. * self->kpqp - 2. * self->kkp_T - self->kpqp_T ) * self->kd + self->kpqp * self->kd_T + self->kqp * self->kpd_T - 2.*self->kkp * self->kd_T ) -
                      self->Dminus * ( ( 2. * self->kkp - self->kpqp_T - self->kkp_T ) * self->qpd + 2. * self->kkp * self->qpd_T - self->kpqp * self->kd_T - self->kqp * self->kpd_T ) );

  self->CUUI = -2. * cos( phi * self->RAD ) * ( self->Dplus * ( 2. * self->kkp * self->kd_T - self->kpqp * self->kd_T - self->kqp * self->kpd_T + 4. * self->xi * self->kkp * self->kP_T - 2. * self->xi * self->kpqp * self->kP_T - 2. * self->xi * self->kqp * self->kpP_T ) -
                 self->Dminus * ( self->kkp * self->qpd_T - self->kpqp * self->kd_T - self->kqp * self->kpd_T + 2. * self->xi * self->kkp * self->qpP_T - 2. * self->xi * self->kpqp * self->kP_T - 2. * self->xi * self->kqp * self->kpP_T ) );

  // Convert Unpolarized Coefficients to nano-barn and use Defurne's Jacobian
  self->con_AUUI = self->AUUI * self->GeV2nb * self->jcob;
  self->con_BUUI = self->BUUI * self->GeV2nb * self->jcob;
  self->con_CUUI = self->CUUI * self->GeV2nb * self->jcob;

  //Unpolarized Coefficients multiplied by the Form Factors
  self->iAUU = (self->Gamma/(fabs(self->t) * self->QQ)) * self->con_AUUI * ( F1 * ReH + self->tau * F2 * ReE );
  self->iBUU = (self->Gamma/(fabs(self->t) * self->QQ)) * self->con_BUUI * ( F1 + F2 ) * ( ReH + ReE );
  self->iCUU = (self->Gamma/(fabs(self->t) * self->QQ)) * self->con_CUUI * ( F1 + F2 ) * ReHtilde;

  // Unpolarized BH-DVCS interference cross section
  self->xIUU = self->iAUU + self->iBUU + self->iCUU;

  return -1. * self->xIUU;
}


#endif
