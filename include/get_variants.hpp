#pragma once
#include <savvy/reader.hpp>

#include <istream>
#include <string>

#include <range/v3/range_fwd.hpp>

#include <range/v3/iterator/default_sentinel.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/facade.hpp>
#include <vector>
#include <range/v3/detail/prologue.hpp>


// namespace ranges {
//   /// \addtogroup group-views
//     /// @{
//     struct variants_index_view : view_facade<variants_index_view, finite>
//     {
//     private:
//         friend range_access;
//       savvy::reader * sin_;
//       std::vector<int> index;
//       savvy::variant str_;
//         struct cursor
//         {
//         private:
//             friend range_access;
//             using single_pass = std::true_type;
//             variants_index_view * rng_ = nullptr;

//         public:
//             cursor() = default;
//             explicit cursor(variants_index_view * rng)
//               : rng_(rng)
//             {}
//             void next()
//             {
//                 rng_->next();
//             }
//           savvy::variant & read() const noexcept
//             {
//                 return rng_->str_;
//             }
//             bool equal(default_sentinel_t) const
//             {
//                 return !rng_->sin_;
//             }
//             bool equal(cursor that) const
//             {
//                 return !rng_->sin_ == !that.rng_->sin_;
//             }
//         };
//         void next()
//         {
//           if(!sin_->read(str_))
//             sin_ = nullptr;
//         }
//         cursor begin_cursor()
//         {
//             return cursor{this};
//         }

//     public:
//         variants_index_view() = default;
//       variants_index_view(savvy::reader & sin)
//           : sin_(&sin)
//           , str_{}
//         {
//             this->next(); // prime the pump
//         }
//       savvy::variant & cached() noexcept
//         {
//             return str_;
//         }
//     };


//     struct getvariants_fn
//     {
//       variants_index_view operator()(savvy::reader & sin) const
//         {
//             return variants_index_view{sin};
//         }
//     };

//     RANGES_INLINE_VARIABLE(getvariants_fn, getvariants)
// }



namespace ranges {
  /// \addtogroup group-views
    /// @{
    struct getvariants_view : view_facade<getvariants_view, unknown>
    {
    private:
        friend range_access;
      savvy::reader * sin_;
      savvy::variant str_;
        struct cursor
        {
        private:
            friend range_access;
            using single_pass = std::true_type;
            getvariants_view * rng_ = nullptr;

        public:
            cursor() = default;
            explicit cursor(getvariants_view * rng)
              : rng_(rng)
            {}
            void next()
            {
                rng_->next();
            }
          savvy::variant & read() const noexcept
            {
                return rng_->str_;
            }
            bool equal(default_sentinel_t) const
            {
                return !rng_->sin_;
            }
            bool equal(cursor that) const
            {
                return !rng_->sin_ == !that.rng_->sin_;
            }
        };
        void next()
        {
          if(!sin_->read(str_))
            sin_ = nullptr;
        }
        cursor begin_cursor()
        {
            return cursor{this};
        }

    public:
        getvariants_view() = default;
      getvariants_view(savvy::reader & sin)
          : sin_(&sin)
          , str_{}
        {
            this->next(); // prime the pump
        }
      savvy::variant & cached() noexcept
        {
            return str_;
        }
    };


    struct getvariants_fn
    {
      getvariants_view operator()(savvy::reader & sin) const
        {
            return getvariants_view{sin};
        }
    };

    RANGES_INLINE_VARIABLE(getvariants_fn, getvariants)
}



#include <range/v3/detail/epilogue.hpp>
