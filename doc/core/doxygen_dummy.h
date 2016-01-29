// \file prevent lint detector warning
// dummy file to trick doxygen into documenting shared pointers
namespace std {
//! Shared pointer dummy
template<class T>
class shared_ptr
{
     T *p; //!< Pointer
};

//! Unique pointer dummy
template<class T>
class unique_ptr
{
     T *p; //!< Pointer
}
