//
//  main.cpp
//  wu_collage_professional
//
//  Created by Zhipeng Wu on 8/20/12.
//  Copyright (c) 2012 Zhipeng Wu. All rights reserved.
//

#include "wu_collage_professional.h"


#include <iostream>
#include <time.h>
#include <stdlib.h>

int main(int argc, const char * argv[])
{
  std::cout << "Well come to \"Collage Professional\"" << std::endl << std::endl;;
  
  if (argc != 3) {
    std::cout << "Error number of input arguments" << std::endl;
    return 0;
  }
  std::string image_list_stress(argv[2]);
  std::string image_list_regular(argv[1]);
  int canvas_width = 0;
  while ((canvas_width < 100) || (canvas_width > 2000)) {
    std::cout << "canvas_width [100, 2000]: ";
    std::cin >> canvas_width;
  }
  int canvas_height = 0;
  while ((canvas_height < 100) || (canvas_height > 2000)) {
    std::cout << "canvas_height [100, 2000]: ";
    std::cin >> canvas_height;
  }
  float streess_ratio = 0;
  while ((streess_ratio < 1) || (streess_ratio > 20)) {
    std::cout << "streess_ratio [1, 20]: ";
    std::cin >> streess_ratio;
  }

  clock_t start, end;
  /****************************************************************************/
  CollageProfessional my_collageHV(image_list_stress,
                                 image_list_regular,
                                 streess_ratio,
                                 canvas_height,
                                 canvas_width);
  start = clock();
  int success = my_collageHV.CreateCollageHV(canvas_width, canvas_height);
  if (success == -1) {
    return -1;
  }
  end = clock();
  cv::Mat canvasHV = my_collageHV.OutputCollageImage();
  std::cout << "-----------HV----------" << std::endl;
  std::cout << "canvas_width: " << my_collageHV.canvas_width() << std::endl;
  std::cout << "canvas_height: " << my_collageHV.canvas_height() << std::endl;
  std::cout << "canvas_alpha: " << my_collageHV.canvas_alpha() << std::endl;
  std::cout << "processing time: " << (end - start) * 1000000 / CLOCKS_PER_SEC
  << " us (10e-6 s)" << std::endl;
  
  float c1 = my_collageHV.c1();
  float c2 = my_collageHV.c2();
  std::cout << "---c1---: " << c1 << std::endl << "---c2---: " << c2 <<std::endl;
  std::string html_save_pathHV = "/tmp/collage_resultHV.html";
  my_collageHV.OutputCollageHtml(html_save_pathHV);
  cv::imshow("Collage", canvasHV);
  cv::waitKey();
  /****************************************************************************/
  CollageProfessional my_collageVH(image_list_stress,
                                   image_list_regular,
                                   streess_ratio,
                                   canvas_height,
                                   canvas_width);
  start = clock();
  success = my_collageVH.CreateCollageVH(canvas_width, canvas_height);
  if (success == -1) {
    return -1;
  }
  end = clock();
  cv::Mat canvasVH = my_collageVH.OutputCollageImage();
  std::cout << "-----------VH----------" << std::endl;
  std::cout << "canvas_width: " << my_collageVH.canvas_width() << std::endl;
  std::cout << "canvas_height: " << my_collageVH.canvas_height() << std::endl;
  std::cout << "canvas_alpha: " << my_collageVH.canvas_alpha() << std::endl;
  std::cout << "processing time: " << (end - start) * 1000000 / CLOCKS_PER_SEC
  << " us (10e-6 s)" << std::endl;
  
  c1 = my_collageVH.c1();
  c2 = my_collageVH.c2();
  std::cout << "---c1---: " << c1 << std::endl << "---c2---: " << c2 <<std::endl;
  std::string html_save_pathVH = "/tmp/collage_resultVH.html";
  my_collageVH.OutputCollageHtml(html_save_pathVH);
  cv::imshow("Collage", canvasVH);
  cv::waitKey();
  /****************************************************************************/

  return 0;
}
