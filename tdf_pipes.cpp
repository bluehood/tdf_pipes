#include <ROOT/TDataFrame.hxx>
using namespace ROOT::Experimental;

template <typename Node, typename Op>
auto operator|(TDF::TInterface<Node> &&d, Op &&op) -> decltype(op(std::forward<TDF::TInterface<Node>>(d)))
{
   return op(std::forward<TDF::TInterface<Node>>(d));
}

template <typename Op>
auto operator|(TDataFrame &&d, Op &&op) -> decltype(op(std::move(d)))
{
   return op(std::move(d));
}

template <typename Op>
auto operator|(TDataFrame &d, Op &&op) -> decltype(op(d))
{
   return op(d);
}

struct DefineColumns {
   template <typename TDF>
   auto operator()(TDF &&d) -> decltype(d.Define(std::string(), std::declval<double (*)(void)>()))
   {
      return d.Define("x", [] { return 42; }).Define("y", [] { return 4.2; });
   }
};

struct ApplyCuts {
   static bool myCut(int x) { return x == 42; }

   template <typename TDF>
   auto operator()(TDF &&d) -> decltype(d.Filter(myCut))
   {
      return d.Filter(myCut, {"x"}, "filter");
   }
};

struct GetHistos {
   using Histos = std::vector<ROOT::Detail::TDF::TResultProxy<TH1D>>;

   template <typename TDF>
   auto operator()(TDF &&d) -> Histos
   {
      Histos hs;
      hs.emplace_back(d.template Histo1D<int>("x"));
      hs.emplace_back(d.template Histo1D<double>("y"));
      return hs;
   }
};

int main()
{
   TDataFrame d(10);

   auto histos = d | DefineColumns() | ApplyCuts() | GetHistos();

   d.Report();

   return 0;
}
