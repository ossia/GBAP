#include "GBAP.hpp"

#include <score/tools/Debug.hpp>

#include <QDebug>

#include <algorithm>

namespace spat
{
void GBAP::operator()(halp::tick t)
{
  bool multi = !inputs.MultiTab.value.empty(); // Check if multiple cursors are used

  const int nSinks = (inputs.nSinksX.value * inputs.nSinksY.value);
  float minX{0}, maxX{1}, minY{0}, maxY{1};
  int ModmX{0}, ModMX{0}, ModmY{0}, ModMY{0};

  volumes.clear(); // Clear previous output

  for (int i = 0; i< (multi ? inputs.MultiTab.value.size() : 1); i++){

    volumes.push_back(ossia::value());
    volumes[i]=std::vector<ossia::value>();
    auto& vol =  volumes[i].get<std::vector<ossia::value>>();

    float posX = multi ? inputs.MultiTab.value[i][0] : inputs.pos.value.x;
    float posY = multi ? inputs.MultiTab.value[i][1]  : inputs.pos.value.y;

    const auto cursorSize = inputs.cursorSize.value;
    auto sinkSize = inputs.sinkSize.value;
    sinkSize.x = std::max(0.01f, sinkSize.x);
    sinkSize.y = std::max(0.01f, sinkSize.y);

    //detects cursor position relative to sinks in X axis
    if(inputs.nSinksX.value > 1)
    {
      const auto den = (sinkSize.x + intervX);
      if(den <= 0.f)
       return;

      minX = std::clamp(posX - cursorSize.x / 2, 0.f, 0.999999f);
      float ModminX = (float)minX / den;
      ModmX = std::floor(ModminX) * 2;
      if(ModminX - (float)ModmX / 2 >= sinkSize.x / den)
        ModmX++;
      maxX = std::clamp(posX + cursorSize.x / 2, 0.f, 0.999999f);
      float ModmaxX = (float)maxX / den;
      ModMX = std::floor(ModmaxX) * 2;
      if(ModmaxX - (float)ModMX / 2 >= sinkSize.x / den)
        ModMX++;
    }

    //detects cursor position relative to sinks in X axis
    if(inputs.nSinksY.value > 1)
    {
      const auto den = (sinkSize.y + intervY);
      if(den <= 0.f)
      return;

      minY = std::clamp(posY - cursorSize.y / 2, 0.f, 0.999999f);
      float ModminY = (float)minY / den;
      ModmY = std::floor(ModminY) * 2;
      if(ModminY - (float)ModmY / 2 >= sinkSize.y / den)
        ModmY++;
      maxY = std::clamp(posY + cursorSize.y / 2, 0.f, 0.999999f);
      float ModmaxY = (float)maxY / den;
      ModMY = std::floor(ModmaxY) * 2;
      if(ModmaxY - (float)ModMY / 2 >= sinkSize.y / den)
        ModMY++;
    }

    if(nSinks <= 1)
    {
      vol.clear();
      nSinksprev = 0;
    }

    if(vol.size() != nSinks)
    {
      vol.clear();
      vol.resize(nSinks); // They will be initialized to zero
    }

    SCORE_ASSERT(vol.size() == nSinks);
    for (int v = 0; v < vol.size(); v++) {
      vol[v] = float(0);
    }

    for(float y = ModmY; y <= ModMY; y++)
    {
      for(float x = ModmX; x <= ModMX; x++)
      {
        int modType = int(x) % 2 + 2 * (int(y) % 2);
        float area = 0.f;
        float center = 0.5f;
        float Xs, Xe, Ys, Ye, Xm, Ym;

        switch(modType)
        {
          case 0: // The cursor is fully inside a single sink
            Xs = std::max(minX, (sinkSize.x + intervX) * x / 2);
            Xe = std::min(((sinkSize.x + intervX) * x + 2 * sinkSize.x) / 2, maxX);
            Ys = std::max(minY, (sinkSize.y + intervY) * y / 2);
            Ye = std::min(((sinkSize.y + intervY) * y + 2 * sinkSize.y) / 2, maxY);
            area = (Xe - Xs) / sinkSize.x * (Ye - Ys) / sinkSize.y;
            vol[x / 2 + y / 2 * inputs.nSinksX.value].get<float>() += area;

            break;

          case 1: // The cursor is between two sinks horizontally
            if(inputs.nSinksX.value > 1 && intervX > 0.f)
            {
              Xm = ((sinkSize.x + intervX) * (x - 1) + 2 * sinkSize.x) / 2;
              Xs = std::max(minX, ((sinkSize.x + intervX) * (x - 1) + 2 * sinkSize.x) / 2);
              Xe = std::min((sinkSize.x + intervX) * std::floor((x + 1) / 2.f), maxX);
              Ys = std::max(minY, (sinkSize.y + intervY) * y / 2);
              Ye = std::min(((sinkSize.y + intervY) * y + 2 * sinkSize.y) / 2, maxY);
              area = (Xe - Xs) / intervX * (Ye - Ys) / sinkSize.y;
              center = ((Xe + Xs) / 2 - Xm) / intervX;
              vol[std::floor((float)x / 2) + y / 2 * inputs.nSinksX.value].get<float>() += (1 - center) * area;
              vol[std::ceil((float)x / 2) + y / 2 * inputs.nSinksX.value].get<float>() += center * area;

            }
            break;

          case 2: // The cursor is between two sinks vertically
            if(inputs.nSinksY.value > 1 && intervY > 0.f)
            {
              Ym = ((sinkSize.y + intervY) * (y - 1) + 2 * sinkSize.y) / 2;
              Xs = std::max(minX, (sinkSize.x + intervX) * x / 2);
              Xe = std::min(((sinkSize.x + intervX) * x + 2 * sinkSize.x) / 2, maxX);
              Ys = std::max(minY, ((sinkSize.y + intervY) * (y - 1) + 2 * sinkSize.y) / 2);
              Ye = std::min((sinkSize.y + intervY) * std::floor((y + 1) / 2.f), maxY);
              area = (Xe - Xs) / sinkSize.x * (Ye - Ys) / intervY;
              center = ((Ye + Ys) / 2 - Ym) / intervY;
              vol[x / 2 + std::floor((float)y / 2) * inputs.nSinksX.value].get<float>() += (1 - center) * area;
              vol[x / 2 + std::ceil((float)y / 2) * inputs.nSinksX.value].get<float>() += center * area;

            }
            break;

          case 3: // The cursor is between four sinks
            if(inputs.nSinksY.value > 1 && inputs.nSinksX.value > 1 && intervY > 0.f && intervX > 0.f)
            {
              Xm = ((sinkSize.x + intervX) * (x - 1) + 2 * sinkSize.x) / 2;
              Ym = ((sinkSize.y + intervY) * (y - 1) + 2 * sinkSize.y) / 2;
              Xs = std::max(minX, ((sinkSize.x + intervX) * (x - 1) + 2 * sinkSize.x) / 2);
              Xe = std::min((sinkSize.x + intervX) * std::floor((x + 1) / 2.f), maxX);
              Ys = std::max(minY, ((sinkSize.y + intervY) * (y - 1) + 2 * sinkSize.y) / 2);
              Ye = std::min((sinkSize.y + intervY) * std::floor((y + 1) / 2.f), maxY);
              area = (Xe - Xs) / intervX * (Ye - Ys) / intervY;
              float centerX = ((Xe + Xs) / 2 - Xm) / intervX;
              float centerY = ((Ye + Ys) / 2 - Ym) / intervY;
              vol[std::floor((float)x / 2) + std::floor((float)y / 2) * inputs.nSinksX.value].get<float>() += (1.f - centerX) * (1.f - centerY) * area;
              vol[std::ceil((float)x / 2) + std::floor((float)y / 2) * inputs.nSinksX.value].get<float>() += centerX * (1.f - centerY) * area;
              vol[std::floor((float)x / 2) + std::ceil((float)y / 2) * inputs.nSinksX.value].get<float>() += (1.f - centerX) * centerY * area;
              vol[std::ceil((float)x / 2) + std::ceil((float)y / 2) * inputs.nSinksX.value].get<float>() += centerX * centerY * area;

            }
            break;
        }
      }
    }

    rollOffArray(vol);
    if (inputs.normalize){
      normalizeArray(vol);
    }

    const float gain = inputs.gain.value
                       * ((inputs.weights.value.size() > 0)
                              ? inputs.weights.value[inputs.systemNumber.value - 1]
                              : 1.f);
    mult(vol, gain);
    //vols = vols.concat(volumes);
  }

  nSinksprev = nSinks;
  outputs.weights.value = volumes;

}

//Apply a rolloff to the volume
void GBAP::rollOffArray(std::vector<ossia::value>& arr)
{
  if(rollOffV <= 0.)
    return;

  float sum{0};

  for(ossia::value& f :arr){
    sum += f.get<float>();
  }

  if(sum != 0)
  {
    for(ossia::value& f : arr)
      f.get<float>()  = std::pow((f.get<float>() / sum), rollOffV / 2.);
  }
}

//adjusts volume according to gain
void GBAP::mult(std::vector<ossia::value>& arr, float scal)
{
  for(ossia::value& f : arr)
    f.get<float>() *= scal;
}

//Normalize volume so that the highest volume is always equal to 1
void GBAP::normalizeArray(std::vector<ossia::value>& arr){

  if(arr.empty())
    return;

  float max {0};

  for (ossia::value& f : arr){
    if (f.get<float>() > max){
      max=f.get<float>();
    }
  }

  if (max != 0){
    for(ossia::value& f : arr){
      f.get<float>() /= max;
    }
  }

}

//updates the interval between sinks according to the size and number of sinks
void GBAP::updateInterv(){

  intervX = (inputs.nSinksX.value > 1)
                      ? std::max(0.f,(1 - inputs.sinkSize.value.x) / (inputs.nSinksX.value - 1) - inputs.sinkSize.value.x)
                    : 0;

  intervY = (inputs.nSinksY.value > 1)
                     ? std::max(0.f,(1 - inputs.sinkSize.value.y) / (inputs.nSinksY.value - 1) - inputs.sinkSize.value.y)
                    : 0;
}

}
