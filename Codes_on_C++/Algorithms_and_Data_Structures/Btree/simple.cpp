////////////////////////////////////////////////////////////////

#include <iostream>
#include <print>
#include <vector>
#include <algorithm>

////////////////////////////////////////////////////////////////

class Btree
{
 public:
  Btree(const std::vector<int>& vec) : head_(new Node)
  {    
    for (size_t i = 0; i < 5; i += 1)
    {
      Node* child = new Node;

      for (size_t j = 0; j < 4; j += 1) {
        child->values_.push_back(vec[i * 5 + j]);
      }

      head_->childs_.push_back(child);
    }

    for (size_t i = 4; i < 20; i += 5)
    {
      head_->values_.push_back(vec[i]);
    }
  }

//  ------------------------------------------------------------

  bool Find(int target) const noexcept
  {
    size_t index = 0;

    for (int value : head_->values_)
    {
      if (value == target)
      {
        return true;
      }

      if (target < value)
      {
        break;
      }

      ++index;
    }
    
    for (int value : head_->childs_[index]->values_)
    {
      if (value == target)
      {
        return true;
      }
    }

    return false;
  }

//  ------------------------------------------------------------

  ~Btree() {
    for (Node* node : head_->childs_)
    {
      delete node;
    }

    delete head_;
  }

//  ------------------------------------------------------------

 private:
  struct Node
  {
    std::vector<int> values_;
    std::vector<Node*> childs_;
  };

//  ------------------------------------------------------------

  Node* head_ = nullptr;
};

//  ------------------------------------------------------------

int main()
{
  size_t count = 0;
  std::cin >> count;

  std::vector<int> data(count);
  for (size_t i = 0; i < count; ++i)
  {
    std::cin >> data[i];
  }

  std::cin >> count;

  std::vector<int> queries(count);
  for (size_t i = 0; i < count; ++i)
  {
    std::cin >> queries[i];
  }

//  ------------------------------------------------------------

  std::sort(data.begin(), data.end());

  Btree btree(data);

//  ------------------------------------------------------------

  std::vector<int> found;
  std::vector<int> not_found;

  for (int query : queries)
  {
    if (btree.Find(query))
    {
      found.push_back(query);
    }
    else
    {
      not_found.push_back(query);
    }
  }

//  ------------------------------------------------------------

  std::print("Targets found:");
  for (int res : found) {
    std::print(" {}", res);

  }

  std::print("\n");

  std::print("Targets not found:");
  for (int res : not_found) {
    std::print(" {}", res);

  }
}

////////////////////////////////////////////////////////////////