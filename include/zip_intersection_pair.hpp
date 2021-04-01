#pragma once
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/default_sentinel.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/semiregular_box.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/facade.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/set_algorithm.hpp>
#include <range/v3/detail/prologue.hpp>

namespace ranges
{
    
    
    /// \cond
    namespace detail
    {
        template<bool IsConst, typename Rng1, typename Rng2, typename C, typename P1,
                 typename P2>
        struct zip_intersection_cursor
        {
        private:
            friend struct zip_intersection_cursor<!IsConst, Rng1, Rng2, C, P1, P2>;
            using pred_ref_ = semiregular_box_ref_or_val_t<C, IsConst>;
            using proj1_ref_ = semiregular_box_ref_or_val_t<P1, IsConst>;
            using proj2_ref_ = semiregular_box_ref_or_val_t<P2, IsConst>;
            pred_ref_ pred_;
            proj1_ref_ proj1_;
            proj2_ref_ proj2_;

            template<typename T>
            using constify_if = meta::const_if_c<IsConst, T>;

            using R1 = constify_if<Rng1>;
            using R2 = constify_if<Rng2>;

            iterator_t<R1> it1_;
            sentinel_t<R1> end1_;

            iterator_t<R2> it2_;
            sentinel_t<R2> end2_;

            void satisfy()
            {
                while(it1_ != end1_ && it2_ != end2_)
                {
                    if(invoke(pred_, invoke(proj1_, *it1_), invoke(proj2_, *it2_)))
                        ++it1_;
                    else
                    {
                        if(!invoke(pred_, invoke(proj2_, *it2_), invoke(proj1_, *it1_)))
                            return;
                        ++it2_;
                    }
                }
            }

        public:
            using value_type = range_value_t<R1>;
            using single_pass = meta::or_c<single_pass_iterator_<iterator_t<R1>>,
                                           single_pass_iterator_<iterator_t<R2>>>;

            zip_intersection_cursor() = default;
            zip_intersection_cursor(pred_ref_ pred, proj1_ref_ proj1, proj2_ref_ proj2,
                                    iterator_t<R1> it1, sentinel_t<R1> end1,
                                    iterator_t<R2> it2, sentinel_t<R2> end2)
              : pred_(std::move(pred))
              , proj1_(std::move(proj1))
              , proj2_(std::move(proj2))
              , it1_(std::move(it1))
              , end1_(std::move(end1))
              , it2_(std::move(it2))
              , end2_(std::move(end2))
            {
                satisfy();
            }
            template(bool Other)(
                /// \pre
                requires IsConst && CPP_NOT(Other)) //
                zip_intersection_cursor(
                    zip_intersection_cursor<Other, Rng1, Rng2, C, P1, P2> that)
              : pred_(std::move(that.pred_))
              , proj1_(std::move(that.proj1_))
              , proj2_(std::move(that.proj2_))
              , it1_(std::move(that.it1_))
              , end1_(std::move(that.end1_))
              , it2_(std::move(that.it2_))
              , end2_(std::move(that.end2_))
            {}
            // clang-format off
            std::pair<iter_reference_t<R1>,iter_reference_t<R2>> read() const
            {
                return std::make_pair<iter_reference_t<R1>,iter_reference_t<R2> >(*it1_,*it2_);
            }
            // clang-format on
            void next()
            {
                ++it1_;
                ++it2_;
                satisfy();
            }
            CPP_member
            auto equal(zip_intersection_cursor const & that) const //
                -> CPP_ret(bool)(
                    /// \pre
                    requires forward_range<Rng1>)
            {
                // does not support comparing iterators from different ranges
                return it1_ == that.it1_ && (it2_ == that.it2_);
            }
            bool equal(default_sentinel_t) const
            {
                return (it1_ == end1_) || (it2_ == end2_);
            }
            // clang-format off
            auto CPP_auto_fun(move)()(const)
            (
                return iter_move(it1_)
            )
            // clang-format on
        };

        constexpr cardinality zip_intersection_cardinality(cardinality c1, cardinality c2)
        {
            return (c1 == unknown) || (c2 == unknown)
                       ? unknown
                       : (c1 >= 0 || c1 == finite) || (c2 >= 0 || c2 == finite) ? finite
                                                                                : unknown;
        }
    } // namespace detail
    /// \endcond

    template<typename Rng1, typename Rng2, typename C, typename P1, typename P2>
    using zip_intersection_view =
        detail::set_algorithm_view<Rng1, Rng2, C, P1, P2, detail::zip_intersection_cursor,
                                   detail::zip_intersection_cardinality(
                                       range_cardinality<Rng1>::value,
                                       range_cardinality<Rng2>::value)>;

    namespace views
    {
        struct zip_intersection_base_fn
        {
            template(typename Rng1, typename Rng2, typename C = less,
                     typename P1 = identity, typename P2 = identity)(
                /// \pre
                requires viewable_range<Rng1> AND input_range<Rng1> AND
                    viewable_range<Rng2> AND input_range<Rng2> AND
                    indirect_relation<
                        C,
                        projected<iterator_t<Rng1>, P1>,
                        projected<iterator_t<Rng2>, P2>>)
            zip_intersection_view<all_t<Rng1>, all_t<Rng2>, C, P1, P2>
            operator()(Rng1 && rng1,
                       Rng2 && rng2,
                       C pred = C{},
                       P1 proj1 = P1{},
                       P2 proj2 = P2{}) const
            {
                return {all(static_cast<Rng1 &&>(rng1)),
                        all(static_cast<Rng2 &&>(rng2)),
                        std::move(pred),
                        std::move(proj1),
                        std::move(proj2)};
            }
        };

        struct zip_intersection_fn : zip_intersection_base_fn
        {
            using zip_intersection_base_fn::operator();

            template(typename Rng2, typename C = less, typename P1 = identity,
                     typename P2 = identity)(
                /// \pre
                requires viewable_range<Rng2> AND input_range<Rng2> AND (!range<C>))
            constexpr auto operator()(Rng2 && rng2,
                                      C && pred = C{},
                                      P1 proj1 = P1{},
                                      P2 proj2 = P2{}) const
            {
                return make_view_closure(bind_back(zip_intersection_base_fn{},
                                                   all(rng2),
                                                   static_cast<C &&>(pred),
                                                   std::move(proj1),
                                                   std::move(proj2)));
            }
        };

        /// \relates zip_intersection_fn
        RANGES_INLINE_VARIABLE(zip_intersection_fn, zip_intersection)
    } // namespace views
} // namespace ranges

#include <range/v3/detail/epilogue.hpp>
