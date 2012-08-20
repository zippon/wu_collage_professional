//
//  wu_collage_professional.cpp
//  wu_collage_professional
//
//  Created by Zhipeng Wu on 8/20/12.
//  Copyright (c) 2012 Zhipeng Wu. All rights reserved.
//

#include "wu_collage_professional.h"
#include <math.h>
#include <fstream>
#include <iostream>

void CollageProfessional::ReadImageList(const std::string img_list_stress,
                                        const std::string img_list_regular) {
  std::ifstream input_list_stress(img_list_stress.c_str());
  std::ifstream input_list_regular(img_list_regular.c_str());
  if ((!input_list_stress) || (!input_list_regular)) {
    std::cout << "Error: ReadImageList" << std::endl;
    return;
  }
  while (!input_list_stress.eof()) {
    std::string img_path;
    std::getline(input_list_stress, img_path);
    img_list_stress_.push_back(img_path);
  }
  while (!input_list_regular.eof()) {
    std::string img_path;
    std::getline(input_list_regular, img_path);
    img_list_regular_.push_back(img_path);
  }
  input_list_regular.close();
  input_list_stress.close();
}

// Recursively calculate aspect ratio for all the inner nodes.
// The return value is the aspect ratio for the node.
float CollageProfessional::CalculateAlpha(TreeNode* node) {
  if (!node->is_leaf_) {
    float left_alpha = CalculateAlpha(node->left_child_);
    float right_alpha = CalculateAlpha(node->right_child_);
    if (node->split_type_ == 'v') {
      node->alpha_ = left_alpha + right_alpha;
      return node->alpha_;
    } else if (node->split_type_ == 'h') {
      node->alpha_ = (left_alpha * right_alpha) / (left_alpha + right_alpha);
      return node->alpha_;
    } else {
      std::cout << "Error: CalculateAlpha" << std::endl;
      return -1;
    }
  } else {
    // This is a leaf node, just return the image's aspect ratio.
    return node->alpha_;
  }
}

// Top-down Calculate the image positions in the colage.
bool CollageProfessional::CalculatePositions(TreeNode* node) {
  // Step 1: calculate height & width.
  if (node->parent_->split_type_ == 'v') {
    // Vertical cut, height unchanged.
    node->position_.height_ = node->parent_->position_.height_;
    if (node->child_type_ == 'l') {
      node->position_.width_ = node->position_.height_ * node->alpha_;
    } else if (node->child_type_ == 'r') {
      node->position_.width_ = node->parent_->position_.width_ -
      node->parent_->left_child_->position_.width_;
    } else {
      std::cout << "Error: CalculatePositions step 0" << std::endl;
      return false;
    }
  } else if (node->parent_->split_type_ == 'h') {
    // Horizontal cut, width unchanged.
    node->position_.width_ = node->parent_->position_.width_;
    if (node->child_type_ == 'l') {
      node->position_.height_ = node->position_.width_ / node->alpha_;
    } else if (node->child_type_ == 'r') {
      node->position_.height_ = node->parent_->position_.height_ -
      node->parent_->left_child_->position_.height_;
    }
  } else {
    std::cout << "Error: CalculatePositions step 1" << std::endl;
    return false;
  }
  
  // Step 2: calculate x & y.
  if (node->child_type_ == 'l') {
    // If it is left child, use its parent's x & y.
    node->position_.x_ = node->parent_->position_.x_;
    node->position_.y_ = node->parent_->position_.y_;
  } else if (node->child_type_ == 'r') {
    if (node->parent_->split_type_ == 'v') {
      // y (row) unchanged, x (colmn) changed.
      node->position_.y_ = node->parent_->position_.y_;
      node->position_.x_ = node->parent_->position_.x_ +
      node->parent_->position_.width_ -
      node->position_.width_;
    } else if (node->parent_->split_type_ == 'h') {
      // x (column) unchanged, y (row) changed.
      node->position_.x_ = node->parent_->position_.x_;
      node->position_.y_ = node->parent_->position_.y_ +
      node->parent_->position_.height_ -
      node->position_.height_;
    } else {
      std::cout << "Error: CalculatePositions step 2 - 1" << std::endl;
    }
  } else {
    std::cout << "Error: CalculatePositions step 2 - 2" << std::endl;
    return false;
  }
  
  // Calculation for children.
  if (node->left_child_) {
    bool success = CalculatePositions(node->left_child_);
    if (!success) return false;
  }
  if (node->right_child_) {
    bool success = CalculatePositions(node->right_child_);
    if (!success) return false;
  }
  return true;
}

// Release the memory for binary tree.
void CollageProfessional::ReleaseTree(TreeNode* node) {
  if (node == NULL) return;
  if (node->left_child_) ReleaseTree(node->left_child_);
  if (node->right_child_) ReleaseTree(node->right_child_);
  delete node;
}

cv::Mat CollageProfessional::OutputCollageImage() const {
  // Traverse tree_leaves_ vector. Resize tile image and paste it on the canvas.
  assert(canvas_alpha_ != -1);
  assert(canvas_width_ != -1);
  cv::Mat canvas(cv::Size(canvas_width_, canvas_height_),
                 CV_8UC3,
                 cv::Scalar(0, 0, 0));
  for (int i = 0; i < tree_leaves_.size(); ++i) {
    FloatRect pos = tree_leaves_[i]->position_;
    cv::Rect pos_cv(pos.x_, pos.y_, pos.width_, pos.height_);
    cv::Mat roi(canvas, pos_cv);
    cv::Mat resized_img(pos_cv.height, pos_cv.width, CV_8UC3);
    cv::Mat image = cv::imread(tree_leaves_[i]->img_path_.c_str());
    assert(image.type() == CV_8UC3);
    cv::resize(image, resized_img, resized_img.size());
    resized_img.copyTo(roi);
  }
  return canvas;
}

bool CollageProfessional::OutputCollageHtml(const std::string output_html_path) {
  assert(canvas_alpha_ != -1);
  assert(canvas_width_ != -1);
  std::ofstream output_html(output_html_path.c_str());
  if (!output_html) {
    std::cout << "Error: OutputCollageHtml" << std::endl;
  }
  
  output_html << "<!DOCTYPE html>\n";
  output_html << "<html>\n";
  output_html << "<h1 style=\"text-align:left\">\n";
  output_html << "\tImage Collage\n";
  output_html << "</h1>\n";
  output_html << "<hr //>\n";
  output_html << "\t<body>\n";
  output_html << "\t\t<div style=\"position:absolute;\">\n";
  for (int i = 0; i < tree_leaves_.size(); ++i) {
    output_html << "\t\t\t<a href=\"";
    output_html << tree_leaves_[i]->img_path_;
    output_html << "\">\n";
    output_html << "\t\t\t\t<img src=\"";
    output_html << tree_leaves_[i]->img_path_;
    output_html << "\" style=\"position:absolute; width:";
    output_html << tree_leaves_[i]->position_.width_ - 1;
    output_html << "px; height:";
    output_html << tree_leaves_[i]->position_.height_ - 1;
    output_html << "px; left:";
    output_html << tree_leaves_[i]->position_.x_ - 1;
    output_html << "px; top:";
    output_html << tree_leaves_[i]->position_.y_ - 1;
    output_html << "px;\">\n";
    output_html << "\t\t\t</a>\n";
  }
  output_html << "\t\t</div>\n";
  output_html << "\t</body>\n";
  output_html << "</html>";
  output_html.close();
  return true;
}