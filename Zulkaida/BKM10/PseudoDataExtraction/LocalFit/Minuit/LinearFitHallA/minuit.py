from iminuit import Minuit

import numpy as np
import pandas as pd 
from BHDVCS_fit import DVCSFIT

import matplotlib
import matplotlib.pyplot as plt

import sys
from scipy.stats import chisquare
from scipy.optimize import curve_fit
from scipy.optimize import minimize
from scipy.stats import chisquare
from statistics import median

dv = DVCSFIT()
func_fit = dv.curve_fit2
f = dv.plot_fit
BHC0 = dv.BHplusC0
prop_term = dv.const_term
x_linear = dv.AperB
linear_plot = dv.plot_linear

dats = pd.read_csv('pseudo_BKM10_hallA_t2.csv')

ind = np.array(dats['index'])
k = np.array(dats['k'])
qq = np.array(dats['QQ'])
xb = np.array(dats['x_b'])
t = np.array(dats['t'])
phi = np.array(dats['phi_x'])
F = 1000*np.array(dats['F'])
errF = 1000*np.array(dats['sigmaF'])
F1 = np.array(dats['F1']) 
F2 = np.array(dats['F2'])
gReH = np.array(dats['gReH'])
gReE = np.array(dats['gReE'])
gReHTilde = np.array(dats['gReHTilde'])
gc0fit = np.array(dats['gc0fit'])
gc1fit = np.array(dats['gc1fit'])

df = pd.read_csv("pseudo_BKM10_hallA_t2.csv")
def F2VsPhi(dataframe,SetNum,xdat,cffs):
    TempFvalSilces=dataframe[dataframe["#Set"]==SetNum]
    TempFvals=1000*TempFvalSilces["F"]
    TempFvals_sigma=1000*TempFvalSilces["sigmaF"]
    mask = (TempFvals_sigma[0:24] > 0)
    temp_phi=TempFvalSilces["phi_x"]
    plt.errorbar(temp_phi[mask],TempFvals[mask],TempFvals_sigma[mask],fmt='.',color='blue',label="Data")
    #plt.xlim(0,368)
    temp_unit=(np.max(TempFvals)-np.min(TempFvals))/len(TempFvals)
    plt.ylim(np.min(TempFvals)-temp_unit,np.max(TempFvals)+temp_unit)
    plt.xticks(fontsize=15)
    plt.yticks(fontsize=15)
    plt.legend(loc=4,fontsize=10,handlelength=3)
    plt.title("Local fit with data set #"+str(SetNum),fontsize=20)
    plt.plot(temp_phi[mask], 1000*f(xdat, cffs), 'g--', label='fit')
    file_name = "plot_set_number_{}.png".format(SetNum)
    plt.savefig(file_name)

def F2VsPhi_r(dataframe,SetNum,xdat,cffs): # F minus BH minus C0 term 
    TempFvalSilces=dataframe[dataframe["#Set"]==SetNum]
    TempFvals=1000*TempFvalSilces["F"]
    TempFvals_sigma=1000*TempFvalSilces["sigmaF"]
    mask = (TempFvals_sigma[0:24] > 0)
    temp_phi=TempFvalSilces["phi_x"]
    plt.errorbar(temp_phi[mask],TempFvals[mask] - 1000*BHC0(xdat, cffs),TempFvals_sigma[mask],fmt='.',color='blue',label="Data")
    plt.xlim(0,368)
    #temp_unit=(np.max(TempFvals)-np.min(TempFvals))/len(TempFvals)
    #plt.ylim(np.min(TempFvals)-temp_unit,np.max(TempFvals)+temp_unit)
    plt.xticks(fontsize=15)
    plt.yticks(fontsize=15)
    plt.legend(loc=4,fontsize=10,handlelength=3)
    plt.title("Local fit with data set #"+str(SetNum),fontsize=20)
    plt.plot(temp_phi[mask], 1000*(f(xdat, cffs) - BHC0(xdat, cffs)), 'g--', label='fit')
    file_name = "plot_set_number_{}.png".format(SetNum)
    plt.savefig(file_name)

def F2VsPhi_linear(dataframe, SetNum, reduced_y, reduced_errs, xaxis, par0, par1): # reduced F vs A per B coefficient
    TempFvalSilces=dataframe[dataframe["#Set"]==SetNum]
    TempFvals_sigma=1000*TempFvalSilces["sigmaF"]
    mask = (TempFvals_sigma[0:24] > 0)
    temp_phi_lin = xaxis
    plt.errorbar(temp_phi_lin, reduced_y, reduced_errs,fmt='.',color='blue',label="Data")
    #plt.xlim(0,368)
    #temp_unit=(np.max(TempFvals)-np.min(TempFvals))/len(TempFvals)
    #plt.ylim(np.min(TempFvals)-temp_unit,np.max(TempFvals)+temp_unit)
    plt.xticks(fontsize=15)
    plt.yticks(fontsize=15)
    plt.legend(loc=4,fontsize=10,handlelength=3)
    plt.title("Local fit with data set #"+str(SetNum),fontsize=20)
    plt.plot(temp_phi_lin, linear_plot(xaxis, par0, par1), 'g--', label='fit')
    file_name = "plot_set_number_{}.png".format(SetNum)
    plt.savefig(file_name)


def LSQ(ReH, ReE, ReHtilde, c0fit, c1fit):
    return np.sum((ydat - 1000*func_fit(xdat, ReH, ReE, ReHtilde, c0fit, c1fit)) ** 2 / errs ** 2)

def LSQ_LinearFit(m_par, c_par):
    return np.sum((reduced_y - (m_par * xaxis + c_par)) ** 2 / reduced_errs ** 2)

def MLM(ReH, ReE, ReHtilde, c0fit, c1fit):
    return np.sum(1000*func_fit(xdat, ReH, ReE, ReHtilde, c0fit, c1fit) - ydat + ydat * np.log(ydat/(1000*func_fit(xdat, ReH, ReE, ReHtilde, c0fit, c1fit))))

def MLM2(ReH, ReE, ReHtilde, c0fit, c1fit):
    return np.sum( -1. * ydat * np.log(func_fit(xdat, ReH, ReE, ReHtilde, c0fit, c1fit)))

def MLM3(ReH, ReE, ReHtilde, c0fit, c1fit):
    return np.sum(func_fit(xdat, ReH, ReE, ReHtilde, c0fit, c1fit)  - ydat * np.log(func_fit(xdat, ReH, ReE, ReHtilde, c0fit, c1fit)))

def MLM4(ReH, ReE, ReHtilde, c0fit, c1fit):
    n_tot = 1000*np.sum(ydat)
    f_tot = 1000*np.sum(1000*func_fit(xdat, ReH, ReE, ReHtilde, c0fit, c1fit))
    return -n_tot * np.log(f_tot)  + f_tot -  np.sum(1000 * ydat * np.log(1000*func_fit(xdat, ReH, ReE, ReHtilde, c0fit, c1fit) / np.sum(1000*func_fit(xdat, ReH, ReE, ReHtilde, c0fit, c1fit)) ))
    
def chisq(ReH, ReE, ReHtilde, c0fit, c1fit):
    return np.mean((ydat - 1000*func_fit(xdat, ReH, ReE, ReHtilde, c0fit, c1fit)) ** 2 / errs ** 2)

def MAE(ReH, ReE, ReHtilde, c0fit, c1fit):
    return np.mean(np.abs(ydat - 1000*func_fit(xdat, ReH, ReE, ReHtilde, c0fit, c1fit))/ydat)

for ii in range(5):
 a = 24*ii # start index of set
 b = a+24 # end index of set
 
 mask = (errF[a:b] > 0)
 xdat = np.array([phi[a:b][mask], qq[a:b][mask], xb[a:b][mask], t[a:b][mask], k[a:b][mask], F1[a:b][mask], F2[a:b][mask]])
 ydat = np.array([F[a:b][mask]])
 errs = np.array([errF[a:b][mask]])

 ## Initial fit to obtain DVCS term

 m = Minuit(LSQ, ReH = 0.0001, ReE = 0.0001, ReHtilde = 0.0001, c0fit = 0.0001, c1fit = 0.) # choose LSQ or MLM
 #m = Minuit(LSQ, ReH = gReH[a+6], ReE = gReE[a+6], ReHtilde = gReHTilde[a+6], c0fit = gc0fit[a+6], c1fit = gc1fit[a+6]) # This is for truth value testing
 ##m.limits = [(-10, 10), (-10, 10), (-10, 10), (0, 10), (0, 10)] # parm limit
 m.errordef = Minuit.LEAST_SQUARES #important for the correct fit error but careful with this
 #m.errordef = Minuit.LIKELIHOOD #important for the correct fit error but careful with this
 #m.errors = (0.0000000001, 0.0000000001,0.0000000001,0.0000000001,0.0000000001) #initial step size
 #m.print_level = 0

 ## for fixing parameter. For twist 2 fix the last parameter
 #m.fixed[0] = True
 #m.fixed[1] = True
 #m.fixed[2] = True
 #m.fixed[3] = True
 m.fixed[4] = True
 m.migrad()
 m.hesse()

 ReH_fit = m.values["ReH"]
 ReE_fit = m.values["ReE"]
 ReHtilde_fit = m.values["ReHtilde"]
 c0fit_fit = m.values["c0fit"]
 c1fit_fit = m.values["c1fit"]  

 ReH_err = m.errors[0]
 ReE_err = m.errors[1]
 ReHT_err = m.errors[2]
 c0fit_fit_err = m.errors[3]
 c1fit_fit_err = m.errors[4]

 ##Testing purposes using truth (debuging tools) ignore this 
 m_truth = F1[a] * gReH[a+6] - t[a] * F2[a] * gReE[a+6] / (4*0.938272*0.938272)
 c_truth = (gReH[a+6] + gReE[a+6]) * (F1[a] + F2[a])
 #cffs = [ReH_fit, ReE_fit, ReHtilde_fit, c0fit_fit, c1fit_fit]
 #####

 # second (linear) fit for twist 2
 ## CFFs from first fit. We only use the c0 term
 cffs = [ReH_fit, ReE_fit, ReHtilde_fit, c0fit_fit, c1fit_fit] 
 reduced_y = ydat - 1000*BHC0(xdat, cffs)
 reduced_y = np.array(prop_term(xdat) * reduced_y)
 reduced_errs = np.array(np.abs(prop_term(xdat) * errs))
 xaxis = x_linear(xdat)  

 m2 = Minuit(LSQ_LinearFit, m_par=0.001, c_par=0.001)
 m2.errordef = Minuit.LEAST_SQUARES
 m2.migrad()
 m2.hesse()

 reduced_y = np.ravel(reduced_y)
 reduced_errs = np.ravel(reduced_errs)

 m_fit = m2.values["m_par"]
 c_fit = m2.values["c_par"]
  
 print('%.4f %.4f %.4f %.4f %.4f %.4f' % (m_truth, m_fit, c_truth, c_fit, gc0fit[a+6], c0fit_fit))

 ##several print option
 #F2VsPhi(df,ii+1,xdat,cffs)
 #F2VsPhi_r(df,ii+1,xdat,cffs)
 F2VsPhi_linear(df,ii+1,reduced_y, reduced_errs, xaxis, m_truth, c_truth)
 plt.clf()

 ##Later after we have complete CFFs
 #cffs = [ReH_fit, ReE_fit, ReHtilde_fit, c0fit_fit, c1fit_fit]
 #chisq_val = chisq(ReH_fit, ReE_fit, ReHtilde_fit, c0fit_fit, c1fit_fit)
 #MAE_val = MAE(ReH_fit, ReE_fit, ReHtilde_fit, c0fit_fit, c1fit_fit)
 #print('%.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.8f %.4f %.4f %.4f' % (ReH_fit, ReH_err, ReE_fit, ReE_err, ReHtilde_fit, ReHT_err, c0fit_fit, c0fit_fit_err, c1fit_fit, c1fit_fit_err, chisq_val, MAE_val))
 
 
