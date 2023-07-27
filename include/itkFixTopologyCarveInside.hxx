/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef itkFixTopologyCarveInside_hxx
#define itkFixTopologyCarveInside_hxx

#include "itkFixTopologyCarveInside.h"
#include "TopologyInvariants.h"

#include "itkBinaryErodeImageFilter.h"
#include "itkFlatStructuringElement.h"
#include "itkNeighborhoodIterator.h"

namespace itk
{

template <class TInputImage, class TOutputImage, class TMaskImage>
TMaskImage *
FixTopologyCarveInside<TInputImage, TOutputImage, TMaskImage>::CreateDefaultMask(ProgressAccumulator * progress)
{
  // if no mask is provided we dilate the input mask
  using kernel_type = itk::FlatStructuringElement<3>;
  auto radius = this->GetRadius();
  auto ball = kernel_type::Ball({ radius, radius, radius }, false);

  auto erode = itk::BinaryErodeImageFilter<MaskImageType, MaskImageType, kernel_type>::New();
  progress->RegisterInternalFilter(erode, 0.1);
  erode->SetInput(this->m_PaddedOutput);
  erode->SetKernel(ball);
  erode->SetForegroundValue(ePixelState::kHardForeground);
  erode->Update();
  return erode->GetOutput();
}

template <class TInputImage, class TOutputImage, class TMaskImage>
void
FixTopologyCarveInside<TInputImage, TOutputImage, TMaskImage>::ComputeThinImage(ProgressAccumulator * accumulator)
{
  using IndexType = typename InputImageType::IndexType;
  using NeighborhoodIteratorType = NeighborhoodIterator<TMaskImage, ConstantBoundaryCondition<TMaskImage>>;

  OutputImagePointer thin_image = this->GetOutput();

  // note: all processing is done on padded_output to avoid
  // checking if index is in region
  auto padded_output = this->m_PaddedOutput;
  auto region = thin_image->GetRequestedRegion();

  // for progress reporting
  size_t                                     mask_size = 0;
  itk::ImageRegionRange<const MaskImageType> image_range(*padded_output, region);
  for (auto && pixel : image_range)
  {
    mask_size += (pixel == ePixelState::kSoftForeground) ? 1 : 0;
  }

  std::vector<IndexType> seeds;
  seeds.reserve(mask_size);
  itk::ImageRegionConstIterator<MaskImageType> it(padded_output, region);
  for (it.GoToBegin(); !it.IsAtEnd(); ++it)
  {
    if (it.Get() == ePixelState::kSoftForeground)
    {
      seeds.push_back(it.GetIndex());
    }
  }

  // process pixels further away from input background first
  // - distance is positive outside
  // - use min priority queue
  using node = std::pair<float, IndexType>;
  const auto cmp = [](const node & l, const node & r) { return l.first > r.first; };
  std::priority_queue<node, std::vector<node>, decltype(cmp)> queue(cmp);
  for (const auto & idx : seeds)
  {
    padded_output->SetPixel(idx, ePixelState::kQueued);
    queue.push(std::make_pair(this->m_DistanceMap->GetPixel(idx), idx));
  }

  auto         neighbors = this->GetNeighborOffsets();
  const size_t num_neighbors = neighbors.size();

  NeighborhoodIteratorType n_it({ 1, 1, 1 }, padded_output, region);

  const auto get_mask = [&n_it](const IndexType & idx, const int FG) {
    n_it.SetLocation(idx);
    auto n = n_it.GetNeighborhood();
    for (auto & v : n.GetBufferReference())
      v = (v != 0) ? FG : 1 - FG;
    return n;
  };

  // erode while topology does not change
  ProgressReporter progress(this, 0, mask_size, 100);
  while (!queue.empty())
  {
    auto idx = queue.top().second; // node
    queue.pop();

    // skip if already processed
    if (padded_output->GetPixel(idx) != ePixelState::kQueued)
      continue;

    auto vals = get_mask(idx, 1);

    // check if point is simple (adding does not change connectivity in the 3x3x3 neighborhood)
    if (topology::EulerInvariant(vals, 1) && topology::CCInvariant(vals, 1) && topology::CCInvariant(vals, 0))
    {
      padded_output->SetPixel(idx, ePixelState::kHardForeground);
    }

    // add unvisited neighbors to queue
    for (size_t k = 0; k < num_neighbors; ++k)
    {
      const IndexType n_id = idx + neighbors[k];
      const float     n_dist = this->m_DistanceMap->GetPixel(n_id);

      if (padded_output->GetPixel(n_id) == ePixelState::kSoftForeground)
      {
        // mark as visited
        padded_output->SetPixel(n_id, ePixelState::kQueued);

        // add to queue
        queue.push(std::make_pair(n_dist, n_id));
      }
    }
    progress.CompletedPixel();
  }

  // copy to output
  InputImagePointer input_image = dynamic_cast<const TInputImage *>(ProcessObject::GetInput(0));
  itk::ImageRegionConstIterator<TInputImage> i_it(input_image, region);
  itk::ImageRegionConstIterator<TMaskImage>  s_it(padded_output, region);
  itk::ImageRegionIterator<TOutputImage>     o_it(thin_image, region);

  for (i_it.GoToBegin(), s_it.GoToBegin(), o_it.GoToBegin(); !s_it.IsAtEnd(); ++i_it, ++s_it, ++o_it)
  {
    o_it.Set(s_it.Get() == ePixelState::kHardForeground ? this->m_InsideValue : i_it.Get());
  }
}

} // end namespace itk

#endif // itkFixTopologyCarveInside_hxx
