#pragma once
#include "cb.h"

namespace CB
{
  template<typename LBL=CB::label, typename LBL_ELM=cb_class>
  char* bufread_label(LBL* ld, char* c, io_buf& cache)
  {
    size_t num = *(size_t*)c;
    ld->costs.clear();
    c += sizeof(size_t);
    size_t total = sizeof(LBL_ELM) * num + sizeof(ld->weight);
    if (cache.buf_read(c, total) < total)
    {
      std::cout << "error in demarshal of cost data" << std::endl;
      return c;
    }
    for (size_t i = 0; i < num; i++)
    {
      LBL_ELM temp = *(LBL_ELM*)c;
      c += sizeof(LBL_ELM);
      ld->costs.push_back(temp);
    }
    memcpy(&ld->weight, c, sizeof(ld->weight));
    c += sizeof(ld->weight);
    return c;
  }

  template <typename LBL = CB::label, typename LBL_ELM = cb_class>
  size_t read_cached_label(shared_data*, void* v, io_buf& cache)
  {
    auto ld = (LBL*)v;
    ld->costs.clear();
    char* c;
    size_t total = sizeof(size_t);
    if (cache.buf_read(c, total) < total)
      return 0;
    bufread_label<LBL,LBL_ELM>(ld, c, cache);

    return total;
  }

  float weight(void*);

  template <typename LBL = CB::label, typename LBL_ELM = cb_class>
  char* bufcache_label(LBL* ld, char* c)
  {
    *(size_t*)c = ld->costs.size();
    c += sizeof(size_t);
    for (size_t i = 0; i < ld->costs.size(); i++)
    {
      *(LBL_ELM*)c = ld->costs[i];
      c += sizeof(LBL_ELM);
    }
    memcpy(c, &ld->weight, sizeof(ld->weight));
    c += sizeof(ld->weight);
    return c;
  }

  template <typename LBL = CB::label, typename LBL_ELM = cb_class>
  void cache_label(void* v, io_buf& cache)
  {
    char* c;
    auto ld = (LBL*)v;
    cache.buf_write(c, sizeof(size_t) + sizeof(LBL_ELM) * ld->costs.size() + sizeof(ld->weight));
    bufcache_label<LBL,LBL_ELM>(ld, c);
  }

  template <typename LBL = CB::label>
  void default_label(void* v)
  {
    auto ld = (LBL*)v;
    ld->costs.clear();
    ld->weight = 1;
  }

  template <typename LBL = CB::label>
  bool is_test_label(void* v)
  {
    auto ld = (LBL*)v;
    if (ld->costs.size() == 0)
      return true;
    for (auto const& cost : ld->costs)
      if (FLT_MAX != cost.cost && cost.probability > 0.)
        return false;
    return true;
  }

  template <typename LBL = CB::label>
  void delete_label(void* v)
  {
    auto ld = (LBL*)v;
    ld->costs.delete_v();
  }

  template <typename LBL = CB::label>
  void copy_label(void* dst, void* src)
  {
    auto ldD = (LBL*)dst;
    auto ldS = (LBL*)src;
    copy_array(ldD->costs, ldS->costs);
    ldD->weight = ldS->weight;
  }
}  // namespace CB
