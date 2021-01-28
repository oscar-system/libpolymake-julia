#include "jlpolymake/jlpolymake.h"
#include <polymake/polytope/beneath_beyond_impl.h>
#include "jlpolymake/type_modules.h"

namespace polymake { namespace polytope {

template <typename T>
T deepcopy(const T& x){
    return T{entire(x)};
}

template <typename Scalar>
Vector<Scalar> deepcopy(const Vector<Scalar>& v){
    return Vector<Scalar>(v.dim(), entire(v));
}

template <typename Row>
ListMatrix<Row> deepcopy(const ListMatrix<Row>& m){
    return ListMatrix<Row>(Matrix<typename Row::value_type>(m));
}

template <typename Scalar>
Matrix<Scalar> deepcopy(const Matrix<Scalar>& m) {
    return Matrix<Scalar>(m.rows(),m.cols(),entire(concat_rows(m)));
}


template <typename E>
class beneath_beyond_algo_stepwise: public beneath_beyond_algo<E>{
    public:
        typedef E value_type;
        typedef const beneath_beyond_algo<E> Base;

        beneath_beyond_algo_stepwise(): Base()
        {
            initialized = false;
        };

        beneath_beyond_algo_stepwise(const beneath_beyond_algo_stepwise<E>& bb) :
            Base(bb),
            initialized{bb.initialized},
            points_added{bb.points_added},
            full_points{deepcopy(bb.full_points)},
            full_linealities{deepcopy(bb.full_linealities)}
        {
            this->AH = deepcopy(bb.AH);
            this->facet_nullspace = deepcopy(bb.facet_nullspace);
            this->interior_points = deepcopy(bb.interior_points);
            this->source_lineality_basis = deepcopy(bb.source_lineality_basis);
            this->points_in_lineality_basis = deepcopy(bb.points_in_lineality_basis);
            this->vertices_this_step = Bitset{};
            this->interior_points_this_step = Bitset{};
            this->visited_facets = Bitset{};
            this->facet_queue = std::deque<Int>{};

            this->transformed_points = deepcopy(bb.transformed_points);
            this->linealities_so_far = deepcopy(bb.linealities_so_far);
            this->lineality_transform = deepcopy(bb.lineality_transform);

            this->vertices_so_far = deepcopy(bb.vertices_so_far);

            // we need this slightly weird copy to make sure to get a proper new graph
            // instead of just an alias
            Int dim = bb.dual_graph.dim();
            this->dual_graph = bb.dual_graph.copy_permuted(sequence(0,dim),sequence(0,dim));

            this->dual_graph.attach(this->facets,entire(bb.facets));
            this->dual_graph.attach(this->ridges,entire(bb.ridges));

            this->source_points = &(this->full_points);
            if (bb.points != bb.source_points)
                this->points = &(this->transformed_points);
            else
                this->points = &(this->full_points);

            this->source_linealities = &(this->full_linealities);
            if (bb.linealities != bb.source_linealities)
                this->linealities = &(this->linealities_so_far);
            else
                this->linealities = &(this->full_linealities);

            for (auto ridge = entire(this->ridges); !ridge.at_end(); ++ridge)
            {
                *ridge = deepcopy(*ridge);
            }

            if (this->make_triangulation)
            {
                using T = typename Base::Triangulation::value_type;
                std::unordered_map<const T*, const T*> triangulation_map;
                auto new_triangulation_iter = this->triangulation.begin();
                for (const auto& simplex : bb.triangulation)
                {
                    auto new_simplex = deepcopy(simplex);
                    *new_triangulation_iter = new_simplex;
                    triangulation_map[&simplex] = &*new_triangulation_iter;
                    new_triangulation_iter++;
                }
                for (auto& fct_info : this->facets)
                {
                    fct_info.normal = deepcopy(fct_info.normal);
                    fct_info.vertices = deepcopy(fct_info.vertices);
                    for (auto& s : fct_info.simplices)
                    {
                        auto new_simplex = triangulation_map[s.simplex];
                        s.simplex = new_simplex;
                    }
                }
            }
        };

        template <typename Iterator>
        void initialize(const Matrix<E>& rays, const Matrix<E>& lins, Iterator perm);
        void initialize(const Matrix<E>& rays, const Matrix<E>& lins)
        {
        #if POLYMAKE_DEBUG
            enable_debug_output();
        #endif
            initialize(rays, lins, entire(sequence(0, rays.rows())));
        };

        void process_point(Int p);

        void finish();

        Int triangulation_size()
        {
            return triangulation.size();
        };

        std::vector<Int> added_points()
        {
            return std::vector<Int>(points_added.begin(), points_added.end());
        }

        // TODO: bundle all results in a structure, move all numbers into it
        template <typename Iterator>
        void compute(const Matrix<E>& rays, const Matrix<E>& lins, Iterator perm);
        void compute(const Matrix<E>& rays, const Matrix<E>& lins)
        {
        #if POLYMAKE_DEBUG
            enable_debug_output();
        #endif
            compute(rays, lins, entire(sequence(0, rays.rows())));
        };

    protected:
        void stop_cleanup();

        using Base::source_points;
        using Base::source_linealities;
        using Base::linealities_so_far;
        using Base::expect_redundant;
        using Base::source_lineality_basis;
        using Base::linealities;
        using Base::transform_points;
        using Base::points;
        using Base::generic_position;
        using Base::triang_size;
        using Base::AH;
        using Base::interior_points;
        using Base::vertices_this_step;
        using Base::interior_points_this_step;
        using Base::facet_normals_valid;
        using Base::facet_normals_low_dim;
        using Base::dual_graph;
        using Base::vertices_so_far;
        using Base::make_triangulation;
        using Base::triangulation;
        using Base::is_cone;
        using Base::facets;
        using compute_state = typename Base::compute_state;
        using Base::state;
        using stop_calculation = typename Base::stop_calculation;
        using typename Base::incident_simplex;
#if POLYMAKE_DEBUG
        using Base::enable_debug_output;
        using Base::do_dump;
        using Base::debug;
        using Base::dump;
#endif

    private :
        bool initialized;
        Bitset points_added;
        Matrix<E> full_points;
        Matrix<E> full_linealities;
};


template <typename E>
template <typename Iterator>
void beneath_beyond_algo_stepwise<E>::initialize(const Matrix<E>& rays, const Matrix<E>& lins, Iterator perm)
{
    full_points = deepcopy(rays);
    full_linealities = deepcopy(lins);
    source_points = &full_points;
    source_linealities = &full_linealities;

    linealities_so_far.resize(0,full_points.cols());

    try {
        if (full_linealities.rows() != 0) {
            if (expect_redundant) {
                source_lineality_basis = basis_rows(full_linealities);
                linealities_so_far = full_linealities.minor(source_lineality_basis, All);
                linealities = &linealities_so_far;
            } else {
                linealities = source_linealities;
            }
            transform_points(); // the only place where stop_calculation could be thrown
        } else {
            points = source_points;
            linealities = expect_redundant ? &linealities_so_far : source_linealities;
        }

        generic_position = !expect_redundant;
        triang_size = 0;
        AH = unit_matrix<E>(points->cols());
        if (expect_redundant) {
            interior_points.resize(points->rows());
            vertices_this_step.resize(points->rows());
            interior_points_this_step.resize(points->rows());
        }

        state = compute_state::zero; // moved from the main compute loop

        points_added = Bitset();
        initialized = true;
    }
    catch (const stop_calculation&) {
#if POLYMAKE_DEBUG
        if (debug >= do_dump) cout << "stop: failed to initialize beneath_beyond_algo" << endl;
#endif
        // TODO: some cleanup??
    }
};

template <typename E>
void beneath_beyond_algo_stepwise<E>::process_point(Int p){
    if ( !points_added.contains(p) ){
        Base::process_point(p);
        points_added += p;
#if POLYMAKE_DEBUG
        if (debug >= do_dump) std::cout << "processed point p = " << p << std::endl;
#endif
    };
};

template <typename E>
template <typename Iterator>
void beneath_beyond_algo_stepwise<E>::compute(const Matrix<E>& rays, const Matrix<E>& lins, Iterator perm){

    initialize(rays, lins);

    try
    {
        for (; !perm.at_end(); ++perm)
            process_point(*perm);
    }
    catch (const stop_calculation&){
#if POLYMAKE_DEBUG
        if (debug >= do_dump) cout << "stop: degenerated to full linear space" << endl;
#endif
        stop_cleanup();
    }

    finish();

#if POLYMAKE_DEBUG
    if (debug >= do_dump) {
        cout << "final ";
        dump();
    }
#endif

};

template <typename E>
void beneath_beyond_algo_stepwise<E>::stop_cleanup(){
    state = compute_state::zero;
    dual_graph.clear();
    vertices_so_far.clear();
    points = source_points;
    interior_points = sequence(0, source_points->rows());
    if (make_triangulation) {
        triangulation.clear();
        triang_size = 0;
    }
}

template <typename E>
void beneath_beyond_algo_stepwise<E>::finish(){

    switch (state) {
    case compute_state::zero:
        if (!is_cone) {
            // empty polyhedron
            AH.resize(0, source_points->cols());
            linealities_so_far.resize(0, source_points->cols());
        }
        break;
    case compute_state::one:
        // There is one empty facet in this case and the point is also a facet normal
        facets[dual_graph.add_node()].normal = points->row(vertices_so_far.front());
        if (make_triangulation) {
            triang_size=1;
            triangulation.push_back(vertices_so_far);
        }
        break;
    case compute_state::low_dim:
        if ( !facet_normals_valid )
        {
            try
            {
                facet_normals_low_dim();
            }
            catch(const stop_calculation& )
            {
                stop_cleanup();
            }
        }
        break;
    case compute_state::full_dim:
        dual_graph.squeeze();
        break;
    }
}

} // of namespace polytope
} // of namespace polymake

template<> struct jlcxx::IsMirroredType<
    polymake::polytope::beneath_beyond_algo<pm::Rational>> : std::false_type { };

namespace jlpolymake {

void add_beneath_beyond(jlcxx::Module& polymake)
{
    polymake
        .add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>>("_BeneathBeyondAlgo")
        .apply<polymake::polytope::beneath_beyond_algo<pm::Rational>>([](auto wrapped) {});

    polymake
        .add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>>("BeneathBeyondAlgo")
        .apply<polymake::polytope::beneath_beyond_algo_stepwise<pm::Rational>>([](auto wrapped) {
            typedef typename decltype(wrapped)::type             WrappedT;
            typedef typename decltype(wrapped)::type::value_type E;
            wrapped.template constructor();

            wrapped.method("_bb_expecting_redundant", &WrappedT::expecting_redundant);
            wrapped.method("_bb_for_cone", &WrappedT::for_cone);
            wrapped.method("_bb_making_triangulation", &WrappedT::making_triangulation);
            wrapped.method("_bb_computing_vertices", &WrappedT::computing_vertices);

            wrapped.method("_bb_compute!", static_cast<
                void (polymake::polytope::beneath_beyond_algo_stepwise<E>::*)(const pm::Matrix<E>&, const pm::Matrix<E>&)
            >(&WrappedT::compute));

            wrapped.method("_bb_initialize!", static_cast<
                void (polymake::polytope::beneath_beyond_algo_stepwise<E>::*)(const pm::Matrix<E>&, const pm::Matrix<E>&)
            >(&WrappedT::initialize));

            wrapped.method("_bb_add_point!", &WrappedT::process_point);
            wrapped.method("_bb_finish!", &WrappedT::finish);

            wrapped.method("facets", &WrappedT::getFacets);
            wrapped.method("vertex_facet_incidence", &WrappedT::getVertexFacetIncidence);
            wrapped.method("affine_hull", &WrappedT::getAffineHull);
            wrapped.method("vertices", &WrappedT::getVertices);
            // wrapped.method("getNonRedundantPoints", &WrappedT::getNonRedundantPoints);
            wrapped.method("non_redundant_linealities", &WrappedT::getNonRedundantLinealities);
            wrapped.method("linealities", &WrappedT::getLinealities);
            // wrapped.method("getDualGraph", &WrappedT::getDualGraph);
            wrapped.method("triangulation", &WrappedT::getTriangulation);
            wrapped.method("triangulation_size", &WrappedT::triangulation_size);
            wrapped.method("state", &WrappedT::added_points);
        });
}

} // of namespace jlpolymake
