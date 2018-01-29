#include <vector>
#include <ROOT/TDataFrame.hxx>
#include <TCanvas.h>
using namespace ROOT::Experimental;

template <typename TDF, typename OP>
auto operator|(TDF &d, OP &&op) -> decltype(op(d))
{
   return op(d);
}

template <typename TDF, typename OP>
auto operator|(TDF &&d, OP &&op) -> decltype(op(d))
{
   return op(d);
}

struct DefineColumns {
   template <typename TDF>
   auto operator()(TDF &d) -> decltype(d.Define(std::string(), std::declval<double (*)(void)>()))
   {
      return d.Define("x", [] { return 42; }).Define("y", [] { return 4.2; });
   }
};

struct ApplyCuts {
   static bool myCut(int x) { return x == 42; }

   template <typename TDF>
   auto operator()(TDF &d) -> decltype(d.Filter(myCut))
   {
      return d.Filter(myCut, {"x"}, "filter");
   }
};

struct GetHistos {
   using Histos = std::vector<ROOT::Detail::TDF::TResultProxy<TH1D>>;

   template <typename TDF>
   auto operator()(TDF &d) -> Histos
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
