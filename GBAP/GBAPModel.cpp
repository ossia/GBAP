#include "GBAP.hpp"

#include <QDebug>

#include <algorithm>

namespace Example
{
void GBAP::operator()(halp::tick t)
{
  float minX{0}, maxX{1}, minY{0}, maxY{1};
  int ModmX{0}, ModMX{0}, ModmY{0}, ModMY{0};

  for (int i = 0; i<inputs.nSources; i++){

    float posX = inputs.pos.value.x;
    float posY = inputs.pos.value.y;

    const auto cursorSize = inputs.sinkSize.value;
    const auto sinkSize = inputs.sinkSize.value;

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

    int nSinks = (inputs.nSinksX.value * inputs.nSinksY.value);
    if(nSinks <= 1)
    {
      volumes.clear();
      nSinksprev = 0;
      return;
    }

    if(nSinksprev != nSinks)
    {
      volumes.clear();
      volumes.resize(nSinks); // They will be initialized to zero
      nSinksprev = nSinks;
    }

    for(int y = ModmY; y <= ModMY; y++)
    {
      for(int x = ModmX; x <= ModMX; x++)
      {
        int modType = x % 2 + 2 * (y % 2);
        float area = 0.f;
        float center = 0.5f;
        float Xs, Xe, Ys, Ye, Xm, Ym;

        switch(modType)
        {
          case 0:
            Xs = std::max(minX, (sinkSize.x + intervX) * x / 2);
            Xe = std::min(((sinkSize.x + intervX) * x + 2 * sinkSize.x) / 2, maxX);
            Ys = std::max(minY, (sinkSize.y + intervY) * y / 2);
            Ye = std::min(((sinkSize.y + intervY) * y + 2 * sinkSize.y) / 2, maxY);
            area = (Xe - Xs) / sinkSize.x * (Ye - Ys) / sinkSize.y;
            volumes[x / 2 + y / 2 * inputs.nSinksX.value] += area;
            break;
          case 1:
            if(inputs.nSinksX.value > 1)
            {
              Xm = ((sinkSize.x + intervX) * (x - 1) + 2 * sinkSize.x) / 2;
              Xs = std::max(minX, ((sinkSize.x + intervX) * (x - 1) + 2 * sinkSize.x) / 2);
              Xe = std::min((sinkSize.x + intervX) * std::floor((x + 1) / 2.f), maxX);
              Ys = std::max(minY, (sinkSize.y + intervY) * y / 2);
              Ye = std::min(((sinkSize.y + intervY) * y + 2 * sinkSize.y) / 2, maxY);
              area = (Xe - Xs) / intervX * (Ye - Ys) / sinkSize.y;
              center = ((Xe + Xs) / 2 - Xm) / intervX;
              volumes[std::floor((float)x / 2) + y / 2 * inputs.nSinksX.value]
                  += (1 - center) * area;
              volumes[std::ceil((float)x / 2) + y / 2 * inputs.nSinksX.value]
                  += center * area;
            }
            break;
          case 2:
            if(inputs.nSinksX.value > 1)
            {
              Ym = ((sinkSize.y + intervY) * (y - 1) + 2 * sinkSize.y) / 2;
              Xs = std::max(minX, (sinkSize.x + intervX) * x / 2);
              Xe = std::min(((sinkSize.x + intervX) * x + 2 * sinkSize.x) / 2, maxX);
              Ys = std::max(minY, ((sinkSize.y + intervY) * (y - 1) + 2 * sinkSize.y) / 2);
              Ye = std::min((sinkSize.y + intervY) * std::floor((y + 1) / 2.f), maxY);
              area = (Xe - Xs) / sinkSize.x * (Ye - Ys) / intervY;
              center = ((Ye + Ys) / 2 - Ym) / intervY;
              volumes[x / 2 + std::floor((float)y / 2) * inputs.nSinksX.value]
                  += (1 - center) * area;
              volumes[x / 2 + std::ceil((float)y / 2) * inputs.nSinksX.value]
                  += center * area;
            }
            break;
          case 3:
            if(inputs.nSinksY.value > 1 && inputs.nSinksX.value > 1)
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
              volumes
                  [std::floor((float)x / 2)
                   + std::floor((float)y / 2) * inputs.nSinksX.value]
                  += (1.f - centerX) * (1.f - centerY) * area;
              volumes
                  [std::ceil((float)x / 2)
                   + std::floor((float)y / 2) * inputs.nSinksX.value]
                  += centerX * (1.f - centerY) * area;
              volumes
                  [std::floor((float)x / 2)
                   + std::ceil((float)y / 2) * inputs.nSinksX.value]
                  += (1.f - centerX) * centerY * area;
              volumes
                  [std::ceil((float)x / 2)
                   + std::ceil((float)y / 2) * inputs.nSinksX.value]
                  += centerX * centerY * area;
            }
            break;
        }
      }
    }
    rollOffArray(volumes);

    const float gain = inputs.gain.value
                       * ((inputs.weights.value.size() > 0)
                              ? inputs.weights.value[inputs.systemNumber.value - 1]
                              : 1.f);
    mult(volumes, gain);
    //vols = vols.concat(volumes);
    //outputs.weights.value[0] = posX;
    //outputs.weights.value[1] = posY;
    outputs.weights.value = volumes;
  }
}

void GBAP::rollOffArray(std::vector<float>& arr)
{
  if(rollOffV <= 0.)
    return;

  float sum = std::accumulate(arr.begin(), arr.end(), 0.f);

  if(sum != 0)
  {
    for(float& f : arr)
      f = std::pow((f / sum), rollOffV / 2.);
  }
}

void GBAP::mult(std::vector<float>& arr, float scal)
{
  for(float& f : arr)
    f *= scal;
}
}

