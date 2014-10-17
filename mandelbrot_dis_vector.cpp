#include <hpx/hpx_main.hpp>
#include <hpx/components/vector/vector.hpp>
#include <hpx/components/vector/algorithm.hpp>
#include <hpx/util/lightweight_test.hpp>
#include <hpx/include/util.hpp>
#include <iostream>
#include <tuple>
#include <math.h>
#include "EasyBMP.h"

#define VAL_TYPE double

typedef std::size_t size_type;
size_t Size_X = 800;
size_t Size_Y = 800;
BMP draw_fractal;
RGBApixel NewColor;
int maxiteration = 400;

// does the pixel calculation 
struct mandelbrot
{
    void operator () (std::tuple<int, double>& t )
    {
        double x  = 0.0;
        double y  = 0.0;
        int times = 0;
        double temp;
        int i = std::get<0>(t)%Size_X;
        int j = std::get<0>(t)/Size_Y;
        double x0 = (double)(i)*3.0/(double)Size_X-2.25;
        double y0 = (double)(j)*2.5/(double)Size_Y-1.25;  
        while ((((x*x)+(y*y))<=4) && (times < maxiteration))
        {
          
            temp  = (x*x)-(y*y) + x0;
            y     = 2*x*y + y0;
            x     = temp;
            times = times+1;  
        }
        std::get<1>(t) =  times;
    }
}mandel;

struct draw_pixel
{
    void operator() (std::tuple<int, double>& t)
    {                           
        if(std::get<1>(t) == maxiteration)
        {                                           
            NewColor.Red = 0;
            NewColor.Green = 0;
            NewColor.Blue = 0;
            NewColor.Alpha = 0;
            draw_fractal.SetPixel(std::get<0>(t)%Size_X, std::get<0>(t)/Size_Y, NewColor);                                                                                                            
        }
        else {
            NewColor.Red = 10;
            NewColor.Green = 11;
            NewColor.Blue = 160;
            NewColor.Alpha = 10;
            draw_fractal.SetPixel(std::get<0>(t)%Size_X, std::get<0>(t)/Size_Y, NewColor);                                                                                                                                                     
        }
    }
}dpixel;

int  main(){
    std::vector<hpx::naming::id_type> localities =  hpx::find_all_localities(); 
    draw_fractal.SetBitDepth(24);         
    draw_fractal.SetSize(Size_X, Size_Y);                  // set image size
    hpx::vector v(640000, hpx::block_cyclic(50,800,localities));

    hpx::util::high_resolution_timer t;
    {
        hpx::for_each(v.begin(), v.end() , mandel);

        std::cout<<"Now placing pixel "<<std::endl;       
     
        hpx::for_each(v.begin(), v.end(), dpixel);      
    }
    std::cout<<"Time it took to do hpx "<<t.elapsed()<<"sec"<<std::endl;
    std::cout<<"Size of Vector  = "<<v.size()<<std::endl;
    draw_fractal.WriteToFile("fractel.bmp");

    return 0;
}
