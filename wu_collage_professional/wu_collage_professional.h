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
                      int canvas_width) {
    ReadImageList(img_list_stress, img_list_regular);
    canvas_height_ = canvas_height;
    canvas_width_ = canvas_width;
    canvas_alpha_ = canvas_width_ / canvas_height_;
    tree_root_ = new TreeNode();
    c1_ = c2_ = -1;
  }
  CollageProfessional(const std::vector<std::string> img_list_stress,
                      const std::vector<std::string> img_list_regular,
                      float stree_ratio,
                      int canvas_height,
                      int canvas_width) {
    for (int i = 0; i < img_list_stress.size(); ++i) {
      img_list_stress_.push_back(img_list_stress[i]);
    }
    for (int i = 0; i < img_list_regular.size(); ++i) {
      img_list_regular_.push_back(img_list_regular[i]);
    }
    canvas_height_ = canvas_height;
    canvas_width_ = canvas_width;
    canvas_alpha_ = canvas_width_ / canvas_height_;
    tree_root_ = new TreeNode();
    c1_ = c2_ = -1;
  }
  ~CollageProfessional() {
    ReleaseTree(tree_root_);
    img_list_regular_.clear();
    img_list_stress_.clear();
  }
  bool CreateCollage();
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
  // Canvas width, this is computed according to canvas_aspect_ratio_.
};
#endif /* defined(__wu_collage_professional__wu_collage_professional__) */
