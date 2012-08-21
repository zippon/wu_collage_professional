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

CollageProfessional::CollageProfessional(const std::string img_list_stress,
                    const std::string img_list_regular,
                    float stress_ratio,
                    int canvas_height,
                    int canvas_width) {
  ReadImageList(img_list_stress, img_list_regular);
  canvas_height_ = canvas_height;
  canvas_width_ = canvas_width;
  canvas_alpha_ = static_cast<float>(canvas_width_) / canvas_height_;
  stress_ratio_ = stress_ratio;
  tree_root_ = new TreeNode();
  c1_ = c2_ = -1;
  stress_collage_ = new CollageAdvanced(img_list_stress_,
                                        canvas_width);
  std::vector<std::string> img_list_regular_1;
  std::vector<std::string> img_list_regular_2;
  for (int i = 0; i < img_list_regular_.size(); ++i) {
    if (i % 2) img_list_regular_1.push_back(img_list_regular_[i]);
    else img_list_regular_2.push_back(img_list_regular_[i]);
  }
  regular_1_collage_ = new CollageAdvanced(img_list_regular_1,
                                           canvas_width);
  regular_2_collage_ = new CollageAdvanced(img_list_regular_2,
                                           canvas_width);
  return;
}
CollageProfessional::CollageProfessional(const std::vector<std::string> img_list_stress,
                    const std::vector<std::string> img_list_regular,
                    float stress_ratio,
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
  stress_ratio_ = stress_ratio;
  canvas_alpha_ = static_cast<float>(canvas_width_) / canvas_height_;
  tree_root_ = new TreeNode();
  c1_ = c2_ = -1;
  stress_collage_ = new CollageAdvanced(img_list_stress_,
                                        static_cast<int>(img_list_stress_.size()));
  std::vector<std::string> img_list_regular_1;
  std::vector<std::string> img_list_regular_2;
  for (int i = 0; i < img_list_regular_.size(); ++i) {
    if (i % 2) img_list_regular_1.push_back(img_list_regular_[i]);
    else img_list_regular_2.push_back(img_list_regular_[i]);
  }
  regular_1_collage_ = new CollageAdvanced(img_list_regular_1,
                                           static_cast<int>(img_list_regular_1.size()));
  regular_2_collage_ = new CollageAdvanced(img_list_regular_2,
                                           static_cast<int>(img_list_regular_2.size()));
  return;
  
}

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

// The function of collage generation.
bool CollageProfessional::CreateCollageHV (float canvas_width, float canvas_height) {
  // Step 1: calculate the size of the stree collage.
  float stress_weight = stress_ratio_ * img_list_stress_.size();
  float regular_weight = img_list_regular_.size();
  float total_weight = stress_weight + regular_weight;
  float ratio = total_weight / stress_weight;
  
  // Step 2: generate stress collage with
  stress_collage_->canvas_width_ = canvas_width_;
  int success = stress_collage_->CreateCollage(canvas_alpha_, ratio);
  if (success == -1) {
    std::cout << "Failure in CreateCollage 1" << std::endl;
  }
  
  float stress_size = stress_collage_->canvas_height() *
                      stress_collage_->canvas_width();
  float amplifier = sqrtf(canvas_width_ * canvas_height_ / ratio / stress_size);
  // Refine the size of stress collage.
  float new_stress_width = stress_collage_->canvas_width() * amplifier;
  float new_stress_height = stress_collage_->canvas_height() * amplifier;
  assert(new_stress_height < canvas_height_);
  assert(new_stress_width < canvas_width_);
  
  // Step 3: generate the other two regular collages.
  // A: h-cut -> v-cut
  float regular_1_width = canvas_width_ - new_stress_width;
  float regular_1_height = new_stress_height;
  float regular_1_alpha = regular_1_width / regular_1_height;
  // float regular_1_size = regular_1_width * regular_1_height;
  float regular_2_width = canvas_width_;
  float regular_2_height = canvas_height_ - new_stress_height;
  float regular_2_alpha = regular_2_width / regular_2_height;
  regular_1_collage_->canvas_width_ = regular_1_width;
  success = regular_1_collage_->CreateCollage(regular_1_alpha, 1.1);
  if (success == -1) {
    std::cout << "Failure in CreateCollage 2" << std::endl;
  }
  regular_2_collage_->canvas_width_ = regular_2_width;
  success = regular_2_collage_->CreateCollage(regular_2_alpha, 1.1);
  if (success == -1) {
    std::cout << "Failure in CreateCollage 3" << std::endl;
  }
  
  
  // Step 4: connect all the three generated collages.
  TreeNode* node = new TreeNode();
  node->split_type_ = 'v';
  node->child_type_ = 'l';
  node->is_leaf_ = false;
  node->left_child_ = stress_collage_->tree_root_;
  stress_collage_->tree_root_->parent_ = node;
  stress_collage_->tree_root_->child_type_ = 'l';
  node->right_child_ = regular_1_collage_->tree_root_;
  regular_1_collage_->tree_root_->parent_ = node;
  regular_1_collage_->tree_root_->child_type_ = 'r';
  node->parent_ = tree_root_;
  tree_root_->left_child_ = node;
  tree_root_->right_child_ = regular_2_collage_->tree_root_;
  regular_2_collage_->tree_root_->parent_ = tree_root_;
  regular_2_collage_->tree_root_->child_type_ = 'r';
  tree_root_->split_type_ = 'h';
  tree_root_->is_leaf_ = false;
  
  // Calculate the aspect ratio for all.
  canvas_alpha_ = CalculateAlpha(tree_root_);
  // Re-calculate canvas_width_ & canvas_height.
  if (canvas_alpha_ >= canvas_width / canvas_height) {
    canvas_width_ = canvas_width;
    canvas_height_ = static_cast<int>(canvas_width_ / canvas_alpha_);
  } else {
    canvas_height_ = canvas_height;
    canvas_width_ = static_cast<int>(canvas_height * canvas_alpha_);
  }
  // Calculate the position again for all.
  tree_root_->position_.x_ = 0;
  tree_root_->position_.y_ = 0;
  tree_root_->position_.height_ = canvas_height_;
  tree_root_->position_.width_ = canvas_width_;
  if (tree_root_->left_child_)
    CalculatePositions(tree_root_->left_child_);
  if (tree_root_->right_child_)
    CalculatePositions(tree_root_->right_child_);
  // Finally add the tree leaves.
  tree_leaves_.clear();
  for (int i = 0; i < stress_collage_->tree_leaves_.size(); ++i) {
    tree_leaves_.push_back(stress_collage_->tree_leaves_[i]);
  }
  for (int i = 0; i < regular_1_collage_->tree_leaves_.size(); ++i) {
    tree_leaves_.push_back(regular_1_collage_->tree_leaves_[i]);
  }
  for (int i = 0; i < regular_2_collage_->tree_leaves_.size(); ++i) {
    tree_leaves_.push_back(regular_2_collage_->tree_leaves_[i]);
  }
  
  c1_ = CalculateC1(canvas_width, canvas_height);
  c2_ = CalculateC2(canvas_width, canvas_height);
  
  return true;
}

bool CollageProfessional::CreateCollageVH (float canvas_width, float canvas_height) {
  // Step 1: calculate the size of the stree collage.
  float stress_weight = stress_ratio_ * img_list_stress_.size();
  float regular_weight = img_list_regular_.size();
  float total_weight = stress_weight + regular_weight;
  float ratio = total_weight / stress_weight;
  
  // Step 2: generate stress collage with
  stress_collage_->canvas_width_ = canvas_width_;
  int success = stress_collage_->CreateCollage(canvas_alpha_, ratio);
  if (success == -1) {
    std::cout << "Failure in CreateCollage 1" << std::endl;
  }
  
  float stress_size = stress_collage_->canvas_height() *
  stress_collage_->canvas_width();
  float amplifier = sqrtf(canvas_width_ * canvas_height_ / ratio / stress_size);
  // Refine the size of stress collage.
  float new_stress_width = stress_collage_->canvas_width() * amplifier;
  float new_stress_height = stress_collage_->canvas_height() * amplifier;
  assert(new_stress_height < canvas_height_);
  assert(new_stress_width < canvas_width_);
  
  // Step 3: generate the other two regular collages.
  // B: v-cut -> h-cut
  float regular_1_width = new_stress_width;
  float regular_1_height = canvas_height_ - new_stress_height;
  float regular_1_alpha = regular_1_width / regular_1_height;
  // float regular_1_size = regular_1_width * regular_1_height;
  float regular_2_width = canvas_width_ - new_stress_width;
  float regular_2_height = canvas_height_;
  float regular_2_alpha = regular_2_width / regular_2_height;
  regular_1_collage_->canvas_width_ = regular_1_width;
  success = regular_1_collage_->CreateCollage(regular_1_alpha, 1.1);
  if (success == -1) {
    std::cout << "Failure in CreateCollage 2" << std::endl;
  }
  regular_2_collage_->canvas_width_ = regular_2_width;
  success = regular_2_collage_->CreateCollage(regular_2_alpha, 1.1);
  if (success == -1) {
    std::cout << "Failure in CreateCollage 3" << std::endl;
  }
  
  
  // Step 4: connect all the three generated collages.
  TreeNode* node = new TreeNode();
  node->split_type_ = 'h';
  node->child_type_ = 'l';
  node->is_leaf_ = false;
  node->left_child_ = stress_collage_->tree_root_;
  stress_collage_->tree_root_->parent_ = node;
  stress_collage_->tree_root_->child_type_ = 'l';
  node->right_child_ = regular_1_collage_->tree_root_;
  regular_1_collage_->tree_root_->parent_ = node;
  regular_1_collage_->tree_root_->child_type_ = 'r';
  node->parent_ = tree_root_;
  tree_root_->left_child_ = node;
  tree_root_->right_child_ = regular_2_collage_->tree_root_;
  regular_2_collage_->tree_root_->parent_ = tree_root_;
  regular_2_collage_->tree_root_->child_type_ = 'r';
  tree_root_->split_type_ = 'v';
  tree_root_->is_leaf_ = false;
  
  // Calculate the aspect ratio for all.
  canvas_alpha_ = CalculateAlpha(tree_root_);
  // Re-calculate canvas_width_ & canvas_height.
  if (canvas_alpha_ >= canvas_width / canvas_height) {
    canvas_width_ = canvas_width;
    canvas_height_ = static_cast<int>(canvas_width_ / canvas_alpha_);
  } else {
    canvas_height_ = canvas_height;
    canvas_width_ = static_cast<int>(canvas_height_ * canvas_alpha_);
  }
  // Calculate the position again for all.
  tree_root_->position_.x_ = 0;
  tree_root_->position_.y_ = 0;
  tree_root_->position_.height_ = canvas_height_;
  tree_root_->position_.width_ = canvas_width_;
  if (tree_root_->left_child_)
    CalculatePositions(tree_root_->left_child_);
  if (tree_root_->right_child_)
    CalculatePositions(tree_root_->right_child_);
  // Finally add the tree leaves.
  tree_leaves_.clear();
  for (int i = 0; i < stress_collage_->tree_leaves_.size(); ++i) {
    tree_leaves_.push_back(stress_collage_->tree_leaves_[i]);
  }
  for (int i = 0; i < regular_1_collage_->tree_leaves_.size(); ++i) {
    tree_leaves_.push_back(regular_1_collage_->tree_leaves_[i]);
  }
  for (int i = 0; i < regular_2_collage_->tree_leaves_.size(); ++i) {
    tree_leaves_.push_back(regular_2_collage_->tree_leaves_[i]);
  }
  
  c1_ = CalculateC1(canvas_width, canvas_height);
  c2_ = CalculateC2(canvas_width, canvas_height);
  
  return true;
}


float CollageProfessional::CalculateC1 (float canvas_width, float canvas_height) {
  float t_1 = stress_ratio_ / (stress_ratio_ *
                               static_cast<int>(img_list_stress_.size() +
                               static_cast<int>(img_list_regular_.size())));
  float t_2 = 1 / (stress_ratio_ *
                   static_cast<int>(img_list_stress_.size() +
                   static_cast<int>(img_list_regular_.size())));
  float canvas_size = static_cast<float>(canvas_height * canvas_width);
  float c1 = 0;
  for (int i = 0; i < stress_collage_->tree_leaves_.size(); ++i) {
    float tile_size = stress_collage_->tree_leaves_[i]->position_.height_ *
    stress_collage_->tree_leaves_[i]->position_.width_;
    float s_1 = tile_size / canvas_size;
    if ((s_1 / t_1) < 0.5) {
      c1 += 5 * (s_1 - t_1) * (s_1 - t_1);
    } else {
      c1 += (s_1 - t_1) * (s_1 - t_1);
    }
  }
  for (int i = 0; i < regular_1_collage_->tree_leaves_.size(); ++i) {
    float tile_size = regular_1_collage_->tree_leaves_[i]->position_.height_ *
    regular_1_collage_->tree_leaves_[i]->position_.width_;
    float s_2 = tile_size / canvas_size;
    if ((s_2 / t_2) < 0.5) {
      c1 += 5 * (s_2 - t_2) * (s_2 - t_2);
    } else {
      c1 += (s_2 - t_2) * (s_2 - t_2);
    }
  }
  return c1;
}
  
float CollageProfessional::CalculateC2 (float canvas_width, float canvas_height) {
  float c2 = 1 - canvas_height_ * canvas_width_ / canvas_height / canvas_width;
  assert(c2 > 0);
  return c2;
}