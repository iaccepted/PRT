#ifndef _SHEVAL_H_
#define _SHEVAL_H_

typedef void(*SHEvalFunc)(const double, const double, const double, double*);
void SHEval3(const double fX, const double fY, const double fZ, double *pSH);
void SHEval4(const double fX, const double fY, const double fZ, double *pSH);
void SHEval5(const double fX, const double fY, const double fZ, double *pSH);
void SHEval6(const double fX, const double fY, const double fZ, double *pSH);
void SHEval7(const double fX, const double fY, const double fZ, double *pSH);
void SHEval8(const double fX, const double fY, const double fZ, double *pSH);
void SHEval9(const double fX, const double fY, const double fZ, double *pSH);
void SHEval10(const double fX, const double fY, const double fZ, double *pSH);

#endif