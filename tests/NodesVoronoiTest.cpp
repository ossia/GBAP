// spat::Nodes in Voronoi mode: the weights vary continuously with the input
// point, including where a node enters or leaves the blend zone.

#include <Nodes/NodesModel.hpp>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cmath>

using Catch::Approx;

namespace
{
spat::Nodes make_nodes()
{
  spat::Nodes n;
  n.inputs.nodes.value
      = {{0.657f, 0.310f, 0.1f}, {0.594f, 0.781f, 0.1f}, {0.156f, 0.412f, 0.1f}};
  n.inputs.smooth.value = 0.18974216f;
  n.inputs.voronoiMode.value = true;
  n.updateNodesFromInput();
  return n;
}

std::vector<float> weights_at(spat::Nodes& n, float x, float y)
{
  n.inputs.inputPoint.value = {x, y};
  n.updateWeights();
  return n.outputs.weights.value;
}
}

TEST_CASE("Nodes: a node fades in from zero as the point leaves a full zone", "[gbap][nodes]")
{
  auto n = make_nodes();
  const auto full = weights_at(n, 0.47f, 0.87f);
  REQUIRE(full.size() == 3);
  CHECK(full[1] == Approx(1.f));

  // Leftwards, through the edge of the blend zone towards the third node.
  float worst = 0.f;
  auto prev = full;
  for(float x = 0.47f; x > 0.15f; x -= 1e-4f)
  {
    const auto w = weights_at(n, x, 0.87f);
    for(int i = 0; i < 3; i++)
      worst = std::max(worst, std::abs(w[i] - prev[i]));
    prev = w;
  }
  CHECK(prev[2] > 0.f);
  CHECK(worst < 1e-3f);
}

TEST_CASE("Nodes: Voronoi weights sum to one", "[gbap][nodes]")
{
  auto n = make_nodes();
  for(float x : {0.1f, 0.3f, 0.5f, 0.7f, 0.9f})
    for(float y : {0.1f, 0.5f, 0.9f})
    {
      float sum = 0.f;
      for(float w : weights_at(n, x, y))
        sum += w;
      CHECK(sum == Approx(1.f));
    }
}
