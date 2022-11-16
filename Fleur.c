#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h> 
#include <time.h>

typedef struct Surface
{
    int width;  ///largeur
    int height; ///hauteur
    int depth; /// bps
    double *data; ///pixels 
}SURFACE;



int iclamp ( int x,int min,int max){
    if (x<min){
        return min;
    }
    if(x>max){
        return max;
    }
    return x;
}


void surface(SURFACE *s,int width,int height){
    s->data = (double *)malloc( 3*width * height * sizeof(double));
    if(s->data !=NULL){
        s->width = width;
        s->height = height;
        s->depth = 8;
    }
}

void _surface(SURFACE *s){
    free(s->data);
}

double *at(SURFACE *s,int x,int y){
    if(x < 0 || x >= s->width) return NULL;
    if(y < 0 || y >= s->height) return NULL;
    return s->data + y * (s->width*3) +(x*3);
    }

void point(SURFACE *s, int x,int y,double r,double g,double b){
    double *pos = at(s,x,s->height-1-y);
    if (pos != NULL){
        pos[0] = r;
        pos[1] = g;
        pos[2] = b; // *(pos+2) = b;
        }   
    }

 void fill(SURFACE *s){
    // Fond bleu
    for (double *i = s-> data, *e=s->data + (3*(s->width * s->height))/2;i!=e;i+=3){
            *i=0.;
            *(i+1)=0.71;
            *(i+2)=1.;
        }
    
    // Fond vertdouble
    for (double *i = s->data + (3*(s->width * s->height))/2, *e=s->data + 3*(s->width * s->height);i!=e;i+=3){
            *i=0.;
            *(i+1)=0.51;
            *(i+2)=0.;
        }
    }
 

 int pgm_write(SURFACE *s, FILE *f){
    int max =1 << s->depth;

    int count = fprintf(f,"P3\n# png_write\n%d %d\n%d\n",s->width,s->height,max-1);

    int cr = s->width;
    for (double *i = s-> data, *e = s->data + 3*(s->width * s->height); i!=e;i++){
        //printf("%f\n",*s->data);
        count +=fprintf(f,"%d",iclamp(*i *max,0,max-1));
        
        if(--cr){
            count+=fprintf(f," ");
        }
        else{
            cr=s->width;
            count+=fprintf(f,"\n");
        }
    }

    return count;
}


/* Transformation géometrique */
//translation d'un point
void translation(double *x,double *y, double valx, double valy){
    *x=*x+valx;
    *y=*y+valy;
}
//homothetie d'un point
void homothetie(double *x,double *y, double valx, double valy){
    *x=*x*valx;
    *y=*y*valy;
}

//rotation d'un point
void rotation(double *x,double *y, double valx, double valy, double angle){
    double x1=*x-valx;
    double y1=*y-valy;
    *x=valx+x1*cos(angle)-y1*sin(angle);
    *y=valy+x1*sin(angle)+y1*cos(angle);
}
/*    DESSIN    */
//Segment
 void segment(SURFACE *s,double x1,double y1,double x2,double y2,double r,double g,double bl){
    int dx,dy;
    dx = x2>x1? x2-x1 : x1-x2;
    dy = y2>y1? y2-y1 : y1-y2;
    if (dx > dy){
        double a = (y2-y1)/(x2-x1);
        double b = y1 - a*x1;
        for (double x = x1; x < x2; x += 1){
            double y = floor(a*x+b);
            point(s, x, y, r, g, bl);
        }
    }
    else{
        double a=(y2-y1)/(x2-x1);
        double b=y1-a*x1;
        if(y1<y2){
            for (double y=y1;y<y2;y+=1){
                double x=floor((y-b)/a);
                point(s, x, y, r, g, bl);
            //printf("%f %f %f\n",r,g,b);

                
            }
        }
        else{
            for (double y=y1;y>y2;y-=1){
            double x=floor((y-b)/a);
            point(s, x, y, r, g, bl);
            //printf("%f %f %f\n",r,g,b);

            }
            }
        }
    } 

//epicycloide avec un k =15
void epicycloide(SURFACE *s,double x,double y){
    // x = (R+r)Cos(t) + rCos(kt) 
    // y = (R+r)Sin(t) + rSin(kt)
    // R etant le rayon du grand cercle et r le rayon des petales
    // random entre 0 et 1 
    double x0, y0, x1, y1;
    double r = 0,g=0,b=0;
    time_t t;
    r = (float)rand()/RAND_MAX;
    g = (float)rand()/RAND_MAX;
    b = (float)rand()/RAND_MAX;
    if(s->width < s->height){
    double rx=s->width*0.03;
    double rTx =s->width*0.05;
    x0 = x+(rTx + rx)*cos(0) + rx*cos(0);
    y0 = y+(rTx+ rx)*sin(0) + rx*sin(0);
    for(double i = 0.001; i <= 2*M_PI ; i += 0.001) {
        x1 = x+(rTx + rx)*cos(i) + rx*cos(15*i);
        y1 = y+(rTx+ rx)*sin(i) + rx*sin(15*i);
        segment(s, x0, y0, x1, y1, r, g, b);
        x0=x1;
        y0=y1;}}
    else{
    double rx=s->height*0.03;
    double rTx =s->height*0.05;
    x0 = x+(rTx + 300)*cos(0) + 300*cos(0);
    y0 = y+(rTx+ 300)*sin(0) + 300*sin(0);
    for(double i = 0.001; i <= 2*M_PI ; i += 0.001) {
        x1 = x+(rTx + rx)*cos(i) + rx*cos(15*i);
        y1 = y+(rTx+ rx)*sin(i) + rx*sin(15*i);
        segment(s, x0, y0, x1, y1, r, g, b);
        x0=x1;
        y0=y1;
    }
    }

}

//Tracer l'epicycloide de l'arbe avec un k=5
void epicycloide_arbre(SURFACE *s,double x,double y){
    // x = (R+r)Cos(t) + rCos(kt) 
    // y = (R+r)Sin(t) + rSin(kt)
    // R etant le rayon du grand cercle et r le rayon des petales
    // random entre 0 et 1 
    double x0, y0, x1, y1;
    if(s->width < s->height){
    double rx=s->width*0.03;
    double rTx =s->width*0.05;
    x0 = x+(rTx + rx)*cos(0) + rx*cos(0);
    y0 = y+(rTx+ rx)*sin(0) + rx*sin(0);
    for(double i = 0.001; i <= 2*M_PI ; i += 0.001) {
        x1 = x+(rTx + rx)*cos(i) + rx*cos(5*i);
        y1 = y+(rTx+ rx)*sin(i) + rx*sin(5*i);
        segment(s, x0, y0, x1, y1, 0.09, 0.50, 0.01);
        x0=x1;
        y0=y1;}}
    else{
    double rx=s->height*0.03;
    double rTx =s->height*0.05;
    x0 = x+(rTx + 300)*cos(0) + 300*cos(0);
    y0 = y+(rTx+ 300)*sin(0) + 300*sin(0);
    for(double i = 0.001; i <= 2*M_PI ; i += 0.001) {
        x1 = x+(rTx + rx)*cos(i) + rx*cos(5*i);
        y1 = y+(rTx+ rx)*sin(i) + rx*sin(5*i);
        segment(s, x0, y0, x1, y1, 0.09, 0.50, 0.01);
        x0=x1;
        y0=y1;
    }
    }

}

// Dessiner un soleil
void soleil(SURFACE *s){
    double x0,y0,x1,y1,x2,y2,x3,y3;
    x0=s->width*0.85+50*cos(0);
    y0=s->height*0.85+50*sin(0);
    for (double i=0.001,j=0;i<=2*M_PI;j++,i+=0.001){
        x1 = s->width*0.85+50*cos(i);
        y1 = s->height*0.85+50*sin(i);
        segment(s,x0,y0,x1,y1,1.,1.,0);
        if ((int)j%644==0){
        x2 = s->width*0.85+80*cos(i);
        y2 = s->height*0.85+80*sin(i);
        segment(s,x1,y1,x2,y2,1.,1.,0);
        segment(s,x2,y2,x1,y1,1.,1.,0);
        }
        x3 = s->width*0.85;
        y3 = s->height*0.85;
        segment(s,x3,y3,x1,y1,1.,1.,0);
        segment(s,x1,y1,x3,y3,1.,1.,0);
        x0=x1;
        y0=y1;
    }
}


// Courbe de bezier qui permet de tracer les feuilles
void courbeBezier_feuille(SURFACE *s, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) {
    double x0, y0;
    x0 = x1;
    y0 = y1;
    for(double i = 0.001; i <= 1; i += 0.001) {
        double x = pow(1-i,3)*x1 + 3*pow(1-i,2)*i*x2 + 3*(1-i)*pow(i,2)*x3 + pow(i,3)*x4;
        double y = pow(1-i,3)*y1 + 3*pow(1-i,2)*i*y2 + 3*(1-i)*pow(i,2)*y3 + pow(i,3)*y4;
        segment(s, x0, y0, x, y,0.19,0.80,0.19);
        x0 = x;
        y0 = y;
    }
}

// Dessiner les 2 feuilles
void feuille(SURFACE *s,double x,double y,double x1,double y1){
    double xt=x<=s->width/2?x*0.4:x;
    double xtt=x<=s->width/2?x*1.4:x;
    if(s->width<s->height){ 
        courbeBezier_feuille(s,x,y,xt*0.60,y*1,xt*0.60,y*2.5,x,y);
        courbeBezier_feuille(s,x,y,xtt*1.40,y*1,xtt*1.40,y*2.5,x,y);}
    else{
        courbeBezier_feuille(s,x,y,xt*0.80,y*1,xt*0.80,y*2.5,x,y);
        courbeBezier_feuille(s,x,y,xtt*1.20,y*1,xtt*1.20,y*2.5,x,y);}

}

// Dessiner la tige de la fleur + appel de la fonction feuille qui va tracer les 2 feuilles
void courbeBezier_tige(SURFACE *s, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) {
    double x0, y0;
    x0 = x1;
    y0 = y1;
    for(double i = 0.01,j=0; i <= 1;j++, i += 0.01) {
        double x = pow(1-i,3)*x1 + 3*pow(1-i,2)*i*x2 + 3*(1-i)*pow(i,2)*x3 + pow(i,3)*x4;
        double y = pow(1-i,3)*y1 + 3*pow(1-i,2)*i*y2 + 3*(1-i)*pow(i,2)*y3 + pow(i,3)*y4;
        if (j==35){
            feuille(s,x,y,0.6*x,y*1.4);
        }
        segment(s, x0, y0, x, y,0.05,1,0.02);
        x0 = x;
        y0 = y;
    }
}
//Dessiner les feuilles de l'arbre
void branche_feuille(SURFACE *s, double x, double y) {
    double x0, y0, x1, y1;
    x0 = x+30*cos(0);
    y0 = y+30*sin(0);
    for(double i = 0.01; i <= 2*M_PI; i += 0.01) {
        x1 = x+50*cos(i);
        y1 = y+50*sin(i);
        epicycloide_arbre(s,x1,y1);
    }
}

// Dessiner un tronc
void tronc(SURFACE *s){
    double x0=s->width*0.40,y0=s->height*0.35;
    double x1=s->width*0.60;
    for(double i=x0;i<=x1;i++){
        segment(s,i,y0,i+1,s->height*0.7,0.50,0.30,0.0);
    }
    branche_feuille(s,(x0+x1)/2,s->height*0.75);

}


// Dessiner une fleur
void fleur(SURFACE *s, double x, double y) {
    epicycloide(s,x,y);
    courbeBezier_tige(s,x,y*0.25,x*1.20,y*0.45,x*1.20,y*0.75,x,y);

}


// Dessiner l'herbe
void feuillage(SURFACE *s){
    //coordonnées de l'herbe
    double x1=0,y1=10;
    double x2=30,y2=30;
    double x3=30,y3=50;
    double x4=10,y4=100;
    //tableau pour rotation
    double *b=(double*)malloc(2*(sizeof(double))),*d=(double*)malloc(2*(sizeof(double))),*c1=(double*)malloc(2*(sizeof(double))),*c2=(double*)malloc(2*(sizeof(double)));
    for (int i=0,j=0;i<1000;j++,i+=10){
        if(j%3==0){ // Translation
        translation(&x1,&y1,20,0);
        translation(&x2,&y2,20,0);
        translation(&x3,&y3,20,0);
        translation(&x4,&y4,20,0);
        courbeBezier_feuille(s,x1,y1,x2,y2,x3,y3,x4,y4);
        courbeBezier_feuille(s,x1+10,y1+30,x2,y2+10,x3,y3+30,x4+10,y4+20);
        }
        if (j%3==1){     // rotation    
        rotation(&x1,&y1,i,60,M_PI);
        rotation(&x2,&y2,i,60,M_PI);
        rotation(&x3,&y3,i,60,M_PI);
        rotation(&x4,&y4,i,60,M_PI);
        courbeBezier_feuille(s,x1,y1,x2,y2,x3,y3,x4,y4);
        courbeBezier_feuille(s,x1+10,y1+30,x2,y2+10,x3,y3+30,x4+10,y4+20);
        }
        if (j%3==2){ // Homothetie
            homothetie(&x1,&y1,1,2);
            homothetie(&x2,&y2,1,2);
            homothetie(&x3,&y3,1,2);
            homothetie(&x4,&y4,1,2);
            courbeBezier_feuille(s,x1,y1,x2,y2,x3,y3,x4,y4);
            courbeBezier_feuille(s,x1+10,y1+30,x2,y2+10,x3,y3+30,x4+10,y4+20);
            homothetie(&x1,&y1,1,0.5);
            homothetie(&x2,&y2,1,0.5);
            homothetie(&x3,&y3,1,0.5);
            homothetie(&x4,&y4,1,0.5);

               
    }
}
}

/* EN COMMUN */
void champ_de_fleur(SURFACE *s){
    double nbr = s->width/200;
    double x = 100, y = s->height*0.45;
    double trslx = s->width*0.2;
    double trsly = s->height*0.2;
    tronc(s);
    for (int i=0;i<nbr;i++){
        fleur(s,x,y);
        if(i%2==0){
            translation(&x,&y,trslx,trsly);
        }
        else{
            translation(&x,&y,trslx,-trsly);
        }
    }
    soleil(s);
    feuillage(s);
}


int main(){

    SURFACE surf={0};
    srand(time(NULL));
    surface(&surf,1000,1000);   
    assert(surf.data!=NULL);
    fill(&surf);
    champ_de_fleur(&surf);
    
    FILE *output = fopen("test.ppm","w");
    assert(output != NULL);
    
    pgm_write(&surf,output);
    
    fclose(output);

    _surface(&surf);

    return 0;

}