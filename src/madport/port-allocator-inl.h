// port of MADlib to Imapala
// author: victor bittorf (bittorf@cs.wisc.edu)

#include <impala_udf/udf.h>
#include <assert.h>

namespace madlib {
namespace port {
namespace dbconn {

/*! \brief A simple allocator
 *
 * MADlib defines an Allocator which is heavily used in the DBConnector
 * codebase. The core functionality of Allocator has been extracted into this
 * simple class and is used by the dbconnector Allocator. This class must
 * implement allocate, free, realloc and a default constructor. Note that
 * the dbconnector Allocator has a SetPortAlloc(PortAllocator&) function, which
 * should be used if the default constructor for PortAllocator is insufficient.
 */

class PortAllocator {
 public:
  static impala_udf::FunctionContext *fallback;
  /*! \brief Default constructor
   * Donot use after calling this ctor, always construct with a context
   */
  PortAllocator() : udfctx_(NULL) { }
  /*! \brief Construct with a backing UDF context to handle allocations
   */
  PortAllocator(impala_udf::FunctionContext* u) : udfctx_(u) { }

  /*! \brief Allocates a pointer, delegated to backing udf context
   * \param s number of bytes to allocate
   */
  void* Allocate(size_t s) const {
    void* p;
    if (udfctx_ == NULL) {
      /* madlib may still use this path some how... */
      //printf(":: rogue alloc\n");
      p = malloc(s);
    } else {
      p = static_cast<void*>(udfctx_->Allocate(s));
    }
    //printf("(new %04lu) %lx\n", s, reinterpret_cast<uint64_t>(p));
    return p;
  }

  /*! \brief Frees a pointer, delegated to backing udf context
   */
  void Free(void*v) const {
    /*
    if (udfctx_ == NULL)
      printf(":: rogue free\n");
    printf("(free)     %lx\n", reinterpret_cast<uint64_t>(v));
    */
    if (udfctx_ == NULL)
      /* madlib may still use this path some how... */
      free(v);
    else
      udfctx_->Free(static_cast<uint8_t*>(v));
    }

  // Do not use
  void* Realloc(void* p, size_t s) const {
    printf("ERROR -- MADlib unexpectedly calling unimplemented reallocate.\n", reinterpret_cast<uint64_t>(p));
    assert(false);
    return NULL;
  }

 private:
  impala_udf::FunctionContext *udfctx_;
};

} // namespace dbconn
} // namespace port
} // namespace madlib
