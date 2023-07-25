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

#ifndef itkFixTopologyBase_hxx
#define itkFixTopologyBase_hxx

#include "itkFixTopologyBase.h"

#include "itkConstantBoundaryCondition.h"
#include "itkFlatStructuringElement.h"
#include "itkImageLinearConstIteratorWithIndex.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionRange.h"
#include "itkNeighborhoodIterator.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"

namespace itk
{

template <class TInputImage, class TOutputImage, class TMaskImage>
FixTopologyBase<TInputImage, TOutputImage, TMaskImage>::FixTopologyBase()
{
  this->SetNumberOfRequiredOutputs(1);
}

template <class TInputImage, class TOutputImage, class TMaskImage>
void
FixTopologyBase<TInputImage, TOutputImage, TMaskImage>::SetMaskImage(const TMaskImage * mask)
{
  this->ProcessObject::SetNthInput(1, const_cast<TMaskImage *>(mask));
}

template <class TInputImage, class TOutputImage, class TMaskImage>
const TMaskImage *
FixTopologyBase<TInputImage, TOutputImage, TMaskImage>::GetMaskImage() const
{
  return itkDynamicCastInDebugMode<MaskImageType *>(const_cast<DataObject *>(this->ProcessObject::GetInput(1)));
}

template <class TInputImage, class TOutputImage, class TMaskImage>
void
FixTopologyBase<TInputImage, TOutputImage, TMaskImage>::PrepareData(ProgressAccumulator * progress)
{
  InputImagePointer  input_image = dynamic_cast<const TInputImage *>(ProcessObject::GetInput(0));
  OutputImagePointer thin_image = this->GetOutput();
  thin_image->SetBufferedRegion(thin_image->GetRequestedRegion());
  thin_image->Allocate();

  // pad by 1 layer so we can get 1x1x1 neighborhood without checking if we are at boundary
  auto region = thin_image->GetRequestedRegion();
  auto padded_region = region;
  padded_region.PadByRadius(1);

  m_PaddedOutput = MaskImageType::New();
  m_PaddedOutput->SetRegions(padded_region);
  m_PaddedOutput->SetSpacing(thin_image->GetSpacing());
  m_PaddedOutput->Allocate();
  m_PaddedOutput->FillBuffer(ePixelState::kBackground);

  ImageRegionConstIterator<TInputImage> it(input_image, region);
  ImageRegionIterator<MaskImageType>    ot(m_PaddedOutput, region);

  // mark the input mask as kHardForeground
  for (it.GoToBegin(), ot.GoToBegin(); !ot.IsAtEnd(); ++it, ++ot)
  {
    if (it.Get() == m_InsideValue)
    {
      ot.Set(ePixelState::kHardForeground);
    }
  }

  // compute distance map: used for priority queue
  auto distance_filter = SignedMaurerDistanceMapImageFilter<MaskImageType, RealImageType>::New();
  progress->RegisterInternalFilter(distance_filter, 0.1);
  distance_filter->SetInput(m_PaddedOutput);
  distance_filter->SetUseImageSpacing(true);
  distance_filter->SetInsideIsPositive(false);
  distance_filter->SetSquaredDistance(false);
  distance_filter->SetBackgroundValue(0);
  distance_filter->Update();
  m_DistanceMap = distance_filter->GetOutput();

  typename MaskImageType::ConstPointer mask_image = GetMaskImage();
  if (!mask_image)
  {
    // if no mask is provided we dilate the input mask
    mask_image = this->CreateDefaultMask(progress);
  }

  // mark dilated as '2', but don't copy padding layer
  ImageRegionConstIterator<MaskImageType> mt(mask_image, region);
  for (mt.GoToBegin(), ot.GoToBegin(); !ot.IsAtEnd(); ++mt, ++ot)
  {
    if (mt.Get() != ot.Get())
    {
      ot.Set(ePixelState::kSoftForeground);
    }
  }
}

template <class TInputImage, class TOutputImage, class TMaskImage>
void
FixTopologyBase<TInputImage, TOutputImage, TMaskImage>::GenerateData()
{
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);

  this->PrepareData(progress);

  this->ComputeThinImage(progress);
}

template <class TInputImage, class TOutputImage, class TMaskImage>
void
FixTopologyBase<TInputImage, TOutputImage, TMaskImage>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace itk

#endif // itkFixTopologyBase_hxx
