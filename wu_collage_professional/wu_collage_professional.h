//
//  wu_collage_professional.h
//  wu_collage_professional
//
//  Created by Zhipeng Wu on 8/20/12.
//  Copyright (c) 2012 Zhipeng Wu. All rights reserved.
//

#ifndef __wu_collage_professional__wu_collage_professional__
#define __wu_collage_professional__wu_collage_professional__

#include <iostream>
#include "wu_collage_advanced.h"

class CollageProfessional {
public:
  CollageProfessional(const std::string img_list_stress,
                      const std::string img_list_regular,
                      float stress_ratio,
                      int canvas_height,
                      int canvas_width);
  CollageProfessional(const std::vector<std::string> img_list_stress,
                      const std::vector<std::string> img_list_regular,
                      float stress_ratio,
                      int canvas_height,
                      int canvas_width);
  ~CollageProfessional() {
    img_list_regular_.clear();
    img_list_stress_.clear();
    tree_leaves_.clear();
    delete stress_collage_;
    delete regular_1_collage_;
    delete regular_2_collage_;
  }
  // The function of collage generation.
  bool CreateCollage(float canvas_width, float canvas_height);
  // Output collage into a single image.
  cv::Mat OutputCollageImage() const;
  // Output collage into a html page.
  bool OutputCollageHtml (const std::string output_html_path);
  
  // Accessors:
  int canvas_height() const {
    return canvas_height_;
  }
  int canvas_width() const {
    return canvas_width_;
  }
  float canvas_alpha() const {
    return canvas_alpha_;
  }
  float c1() const {
    return c1_;
  }
  float c2() const {
    return c2_;
  }
  
private:
  // Read input images from image list.
  void ReadImageList(const std::string img_list_stress,
                     const std::string img_list_regular);
  // Recursively calculate aspect ratio for all the inner nodes.
  // The return value is the aspect ratio for the node.
  float CalculateAlpha(TreeNode* node);
  // Top-down Calculate the image positions in the colage.
  bool CalculatePositions(TreeNode* node);
  // Clean and release the binary_tree.
  void ReleaseTree(TreeNode* node);
  // Calculate costs.
  float CalculateC1 (float canvas_width, float canvas_height);
  float CalculateC2 (float canvas_width, float canvas_height);
  
  // Type 1 cost - coverage of the canvas area.
  float c1_;
  // Type 2 cost - the matching of image sizes in the layout to the input.
  float c2_;
  // Final generation binary tree.
  TreeNode* tree_root_;
  // Vector containing stressed image paths.
  std::vector<std::string> img_list_stress_;
  // Vector containing regular image paths.
  std::vector<std::string> img_list_regular_;
  // Vector containing leaf nodes of the tree.
  std::vector<TreeNode*> tree_leaves_;
  // The real height of generated collage.
  int canvas_height_;
  // The real width of generated collage.
  int canvas_width_;
  // The real aspect ratio of generated collage.
  float canvas_alpha_;
  // The size ratio between a stress image to regular image.
  float stress_ratio_;
  // sub-collages.
  CollageAdvanced* stress_collage_;
  CollageAdvanced* regular_1_collage_;
  CollageAdvanced* regular_2_collage_;
};
#endif /* defined(__wu_collage_professional__wu_collage_professional__) */
