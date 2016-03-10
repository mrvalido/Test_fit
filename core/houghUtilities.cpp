/*
 * houghUtilities.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: mrv
 */


#include "houghUtilities.h"

/*int filterGaussian(valarray<unsigned char> im, int x, int y, int dimX){
	return (   (im[(y)*dimX + (x)]) + 2*(im[(y)*dimX + (x)]) +   (im[(y)*dimX + (x)]) +
			 2*(im[(y)*dimX + (x)]) + 4*(im[(y)*dimX + (x)]) + 2*(im[(y)*dimX + (x)]) +
			   (im[(y)*dimX + (x)]) + 2*(im[(y)*dimX + (x)]) +   (im[(y)*dimX + (x)])
		   )/16;
}

valarray<unsigned char>  gaussian(valarray<unsigned char> im_in){
	valarray<unsigned char> temp(im_in.size());
	int dimX =

	for (int y=1; y < dimY-1; y++){
		for (int x=1; x < dimX-1; x++){
			if (dila[ind( y, x,dimX )] == MAX_BRIGHTNESS_8){
				if (dila[ind( y-1, x,dimX )]==0) 			dila[ind( y-1, x,dimX )] = 2;		//Vecino de la izq
				if (dila[ind( y, x-1,dimX )]==0) 			dila[ind( y, x-1,dimX )] = 2;		//Vecino de arriba
				if (dila[ind( y+1, x,dimX )]==0) 	 		dila[ind( y+1, x,dimX )]= 2;		//Vecino de la derecha
				if (dila[ind( y, x+1,dimX )]==0)			dila[ind( y, x+1,dimX )] = 2;		//Vecino de abajo
			}
		}
	}
}*/

long Sobel(int val1,int val2,int val3,int val4,int val5,int val6){
	return (long)(val1 + 2*val2 + val3 -val4 - 2*val5 - val6);
}

ImageValChar escalado8(ImageValLong val){
	int size_val = val.size();
	ImageValChar temp(size_val);

	unsigned long mx = val.max();

	for(int i = 0; i < size_val; i++){
		temp[i] = (unsigned char) (( (float)(val[i])/(float)mx ) * 255.0);
	}

	return temp;
}


/*
 * Calculate gradient of image using sobel kernel
 *
 * @param im_im 	32 bits grayscale Input image
 *
 * @return image	32 bits grayscale Gradient image
 */
ImageValLong gradient(ImageValInt im_in){

	int size_im = im_in.size();
	long gx, gy;
	ImageValLong temp(size_im);

	int x_y_1;			//(y-1)(x-1)
	int x_1;			//(y)(x-1)
	int x_y_t_1;		//(y+1)(x-1)
	int x_y_t_2;		//(y-1)(x+1)
	int x_2;			//(y)(x+1)
	int x_y_2;			//(y+1)(x+1)
	int y_1;			//(y-1)(x)
	int y_2;			//(y+1)(x)

	cout << "Empieza" << endl;
	for(int y = 1; y < dimY - 1; y++){
		cout << "Bucle " << y << endl;
		for(int x = 1; x < dimX - 1; x++){
			cout << "Bucle X: " << im_in[(y+1)*dimX+ (x-1) ] << endl;



			x_y_1 = im_in[(y-1)*dimX+ (x-1) ];			//(y-1)(x-1)
			y_1 = im_in[(y-1)*dimX+ (x) ];				//(y-1)(x)
			x_y_t_2 = im_in[(y-1)*dimX+ (x+1) ];		//(y-1)(x+1)
			x_1 = im_in[(y)*dimX+ (x-1) ];				//(y)(x-1)
			x_2 = im_in[(y)*dimX+ (x+1) ];				//(y)(x+1)
			x_y_t_1 = im_in[(y+1)*dimX+ (x-1) ];		//(y+1)(x-1)
			y_2 = im_in[(y+1)*dimX+(x) ];				//(y+1)(x)
			x_y_2 = im_in[(y+1)*dimX+(x+1) ];			//(y+1)(x+1)

			cout << "Valores recogidos" << endl;
			gx = Sobel(x_y_1, x_1, x_y_t_1, x_y_t_2, x_2, x_y_2);
			gy = Sobel(x_y_1, y_1, x_y_t_2, x_y_t_1, y_2, x_y_2);

			temp[ind( y, x )] = (unsigned long)(gx*gx + gy*gy);

		}
	}
	cout << "Salimos" << endl;

	return temp;
}


valarray<unsigned int> histogram (ImageValChar val){
	valarray<unsigned int> hist(GRAYLEVEL_8);

	int size_val = val.size();

	for(int i = 0; i < size_val; i++){
		hist[ val[i] ]++;
	}

	return hist;
}

valarray<float> probability (valarray<unsigned int> hist){
	valarray<float> prob(GRAYLEVEL_8);

	int numeroPixels = dimX*dimY;
	for (int x = 0; x < GRAYLEVEL_8; x++) {
		prob[x] = (float)hist[x]/(float)numeroPixels;
	}
	return prob;
}
//--------------------------------------------------------------------------------------------------------------------



/**
 *  Permite obtener el valor del umbral para realizar la binarizacion
 *
 *  @param val		Imagen a la cual le calcularemos el umbral
 *  @return int 	Devolvemos el valor del umbral calculado
 */
int otsu_th(ImageValChar val){
	valarray<unsigned int> hist(GRAYLEVEL_8);
	valarray<float> prob(GRAYLEVEL_8);
	valarray<float> omega(GRAYLEVEL_8); /* prob of graylevels */
	valarray<float> myu(GRAYLEVEL_8);   /* mean value for separation */
	valarray<float> sigma(GRAYLEVEL_8); /* inter-class variance */

	double max_sigma;
	int threshold; /* threshold for binarization */

	hist = histogram (val);

	prob = probability(hist);

	/* omega & myu generation */
	omega[0] = prob[0];
	myu[0] = 0.0;       /* 0.0 times prob[0] equals zero */
	for (int i = 1; i < GRAYLEVEL_8; i++) {
		omega[i] = omega[i-1] + prob[i];
		myu[i] = myu[i-1] + i*prob[i];
	}

	/* sigma maximization
	 sigma stands for inter-class variance
	 and determines optimal threshold value */
	threshold = 0;
	max_sigma = 0.0;
	for (int i = 0; i < GRAYLEVEL_8-1; i++) {
		if (omega[i] != 0.0 && omega[i] != 1.0)
			sigma[i] = pow(myu[GRAYLEVEL_8-1]*omega[i] - myu[i], 2) /
			(omega[i]*(1.0 - omega[i]));
		else
			sigma[i] = 0.0;
		if (sigma[i] > max_sigma) {
			max_sigma = sigma[i];
			threshold = i;
		}
	}

	return threshold;
}


/**
 *  Dada una imagen de entrada la binarizamos en blanco y negro
 *
 *  @param val Imagen de entrada a binarizar
 *
 *  @return valarray<unsigned char> Devolvemos el resultado de la imagen binarizada
 */
ImageValChar binarizar (ImageValChar val, int threshold){
	int size_val = val.size();
	ImageValChar bin(size_val);

	for(int i = 0; i < size_val; i++){
		if (val[i] > threshold){
			bin[i] = MAX_BRIGHTNESS_8;
		}
	}

	return bin;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------


/*
 * Recibimos una imagen en 8 bits y le aplicamos la dilatacion de bordes
 *
 * @param image		Imagen a la cual aplicaremos el Dilate
 * @return image 	Imagen una vez aplicado el filtro
 */

ImageValChar dilate(ImageValChar val){

	ImageValChar dila = val;

	for (int y=1; y < dimY-1; y++){
		for (int x=1; x < dimX-1; x++){
			if (dila[ind( y, x )] == MAX_BRIGHTNESS_8){
				if (dila[ind( y-1, x )]==0) 			dila[ind( y-1, x )] = 2;		//Vecino de la izq
				if (dila[ind( y, x-1 )]==0) 			dila[ind( y, x-1 )] = 2;		//Vecino de arriba
				if (dila[ind( y+1, x )]==0) 	 		dila[ind( y+1, x )] = 2;		//Vecino de la derecha
				if (dila[ind( y, x+1 )]==0)				dila[ind( y, x+1 )] = 2;		//Vecino de abajo
			}
		}
	}

	int size_val = val.size();
	for(int i = 0; i < size_val; i++){
		if (dila[i] == 2){
			dila[i] = MAX_BRIGHTNESS_8;
		}
	}

	return dila;
}
//--------------------------------------------------------------------------------------------------------------------

ImageValChar erode(ImageValChar val){

	ImageValChar ero = val;

	for (int y=1; y < dimY-1; y++){
		for (int x=1; x < dimX-1; x++){
			if (ero[ind( y, x )] == 0){
				if (ero[ind( y-1, x )]==MAX_BRIGHTNESS_8) 			ero[ind( y-1, x )] = 2;		//Vecino de la izq
				if (ero[ind( y, x-1 )]==MAX_BRIGHTNESS_8) 			ero[ind( y, x-1 )] = 2;		//Vecino de arriba
				if (ero[ind( y+1, x )]==MAX_BRIGHTNESS_8) 	 		ero[ind( y+1, x )] = 2;		//Vecino de la derecha
				if (ero[ind( y, x+1 )]==MAX_BRIGHTNESS_8)			ero[ind( y, x+1 )] = 2;		//Vecino de abajo
			}
		}
	}

	int size_val = val.size();
	for(int i = 0; i < size_val; i++){
		if (ero[i] == 2){
			ero[i] = 0;
		}
	}

	return ero;
}
//--------------------------------------------------------------------------------------------------------------------