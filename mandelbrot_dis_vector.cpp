#include <hpx/hpx_main.hpp>
#include <hpx/components/vector/vector.hpp>
#include <hpx/components/vector/algorithm.hpp>
#include <hpx/util/lightweight_test.hpp>
#include <hpx/include/util.hpp>
#include <iostream>

#include <math.h>
#include "EasyBMP.h"

#define VAL_TYPE double


typedef std::size_t size_type;

size_t Size_X = 200;
size_t Size_Y = 200;


struct mandelbrot
{
    void operator () (double& i)
    {
       // std::cout<<"i = "<< i << " ,from locality "<<hpx::find_here()<<std::endl;
        double Zr = 0.0;
        double Zi = 0.0;
        int times = 0;
        double temp;
        int x0 = (int)i%Size_X;
        int y0 = (int)i/Size_Y;
        double Cr = (double)(x0)*3.5/(double)Size_X-2.5;
        double Ci = (double)(y0)*2.0/(double)Size_Y-1.0;
        int maxiteration = 300;
        Zr = Zr+Cr;
        Zi = Zi+Ci;
          
        while ((((Zr*Zr)+(Zi*Zi))<=4) && (times < maxiteration))
        {
          
            temp  = (Zr*Zr)-(Zi*Zi);
            Zi    = 2*Zr*Zi;
            Zr    = temp+Cr;
            Zi    = Zi+Ci;                 
            times = times+1;  
        }
        i =  times;
        std::cout<<i<<std::endl;
    }
}mandel;


int  main(){
    
    BMP draw_fractal;
    draw_fractal.SetBitDepth(24);         
    draw_fractal.SetSize(Size_X, Size_Y);                  // set image size
    RGBApixel NewColor;
    int maxiteration = 300;

    std::vector<hpx::naming::id_type> localities =
            hpx::find_all_localities(); 
    
    hpx::vector v(40000, hpx::block(200,localities));

//    hpx::vector v1(1296, hpx::cyclic(5,localities));

//    hpx::vector v2(7517, hpx::block_cyclic(42,9,localities));
    
//    hpx::vector v3(517, hpx::block_cyclic(47,7,localities));
 
//    hpx::vector v4(89, hpx::block_cyclic(4,4,localities));
    hpx::util::high_resolution_timer t;
    {
      for(size_type i = 0; i < v.size(); ++i)
      {
         v.set_value(i, (VAL_TYPE)i);
      }
      hpx::for_each(v.begin(), v.end() , mandel);

      std::cout<<"Now placing pixel "<<std::endl;
      for(unsigned int k = 0;k<v.size();k++)
      {  
                   
         if(v[k] ==maxiteration)
         {
            NewColor.Red = 0;
            NewColor.Green = 0;
            NewColor.Blue = 0;
            NewColor.Alpha = 0;
            draw_fractal.SetPixel(k%Size_X, k/Size_Y, NewColor);
                
        }
        else {
            NewColor.Red = 10;
            NewColor.Green = 11;
            NewColor.Blue = 160;
            NewColor.Alpha = 10;
            draw_fractal.SetPixel(k%Size_X, k/Size_Y, NewColor);
                                                                
        }                            
                  
          
      
      }
   }
   std::cout<<"Time it tool to do hpx "<<t.elapsed()<<"sec"<<std::endl;
   std::cout<<"Size of Vector  = "<<v.size()<<std::endl;
   draw_fractal.WriteToFile("fractel.bmp");

     
    return 0;
}
