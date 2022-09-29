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
#ifndef itkFixTopologyCarveOutside_h
#define itkFixTopologyCarveOutside_h

#include "itkImageToImageFilter.h"
#include "itkProgressAccumulator.h"

#include <vector>

namespace itk
{
/** \class FixTopologyCarveInside
 *
 * \brief This filter does morphological closing with topology constraints
 *
 * It works by doing following steps:
 * 1. dilate the foreground
 * 2. erode/carve the dilated voxels from the "outside" while preserving the topology of the dilated region.
 *
 * The first step closes holes and the second returns as close as possible to the input mask, while ensuring that the
 * holes are not re-opened.
 *
 * This filter implements ideas from: Vanderhyde, James. "Topology control of volumetric data.", PhD dissertation,
 * Georgia Institute of Technology, 2007..
 *
 * \author Bryn Lloyd
 * \ingroup TopologyControl
 */
template <class TInputImage, class TOutputImage, class TMaskImage = itk::Image<unsigned char, 3>>
class ITK_TEMPLATE_EXPORT FixTopologyCarveOutside : public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Extract dimension from input and output image. */
  itkStaticConstMacro(InputImageDimension, unsigned int, TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int, TOutputImage::ImageDimension);

  static const unsigned int ImageDimension = InputImageDimension;

  /** Standard class typedefs. */
  using Self = FixTopologyCarveOutside;
  using Superclass = ImageToImageFilter<TInputImage, TOutputImage>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Method for creation through the object factory */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(FixTopologyCarveOutside, ImageToImageFilter);

  /** Type for input image. */
  using InputImageType = TInputImage;

  /** Type for output image: Skeleton of the object.  */
  using OutputImageType = TOutputImage;

  /** Type for mask image  */
  using MaskImageType = TMaskImage;

  /** Type for the pixel type of the input image. */
  using InputImagePixelType = typename InputImageType::PixelType;

  /** Type for the pixel type of the input image. */
  using OutputImagePixelType = typename OutputImageType::PixelType;

  /** Pointer Type for input image. */
  using InputImagePointer = typename InputImageType::ConstPointer;

  /** Pointer Type for the output image. */
  using OutputImagePointer = typename OutputImageType::Pointer;

  /** Pointer Type for the mask image. */
  using MaskImageTypePointer = typename MaskImageType::Pointer;

  /** Optional mask (if none is provided the input mask is dilated by 'Radius') */
  void
  SetMaskImage(const TMaskImage * mask);
  const TMaskImage *
  GetMaskImage() const;

  itkSetMacro(Radius, SizeValueType);
  itkGetConstMacro(Radius, SizeValueType);

  itkSetMacro(InsideValue, InputImagePixelType);
  itkGetConstMacro(InsideValue, InputImagePixelType);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(SameDimensionCheck, (Concept::SameDimension<InputImageDimension, 3>));
  itkConceptMacro(SameTypeCheck, (Concept::SameType<InputImagePixelType, OutputImagePixelType>));
  itkConceptMacro(InputAdditiveOperatorsCheck, (Concept::AdditiveOperators<InputImagePixelType>));
  itkConceptMacro(InputConvertibleToIntCheck, (Concept::Convertible<InputImagePixelType, int>));
  itkConceptMacro(IntConvertibleToInputCheck, (Concept::Convertible<int, InputImagePixelType>));
  itkConceptMacro(InputIntComparableCheck, (Concept::Comparable<InputImagePixelType, int>));
  /** End concept checking */
#endif

protected:
  FixTopologyCarveOutside();
  ~FixTopologyCarveOutside() override = default;
  void
  PrintSelf(std::ostream & os, Indent indent) const override;

  void
  GenerateData() override;

  void
  PrepareData(ProgressAccumulator * progress);

  void
  ComputeThinImage(ProgressAccumulator * progress);

  using SpacingType = typename InputImageType::SpacingType;
  using OffsetType = typename InputImageType::OffsetType;

  std::vector<OffsetType>
  GetNeighborOffsets()
  {
    // 18-connectivity
    return { OffsetType{ -1, 0, 0 }, OffsetType{ 1, 0, 0 }, OffsetType{ 0, -1, 0 },  OffsetType{ 0, 1, 0 },
             OffsetType{ 0, 0, -1 }, OffsetType{ 0, 0, 1 }, OffsetType{ -1, -1, 0 }, OffsetType{ 1, -1, 0 },
             OffsetType{ -1, 1, 0 }, OffsetType{ 1, 1, 0 }, OffsetType{ 0, -1, -1 }, OffsetType{ 0, 1, -1 },
             OffsetType{ 0, -1, 1 }, OffsetType{ 0, 1, 1 }, OffsetType{ -1, 0, -1 }, OffsetType{ 1, 0, -1 },
             OffsetType{ -1, 0, 1 }, OffsetType{ 1, 0, 1 } };
  }
  std::vector<float>
  GetNeighborDeltas(const std::vector<OffsetType> & offsets, const SpacingType & spacing);

private:
  ITK_DISALLOW_COPY_AND_ASSIGN(FixTopologyCarveOutside);

  enum ePixelState : OutputImagePixelType
  {
    kBackground = 0,
    kHardForeground,
    kDilated,
    kVisited
  };

  MaskImageTypePointer m_PaddedOutput;

  using RealImageType = itk::Image<float, 3>;
  RealImageType::Pointer m_DistanceMap;

  SizeValueType       m_Radius = 1;
  InputImagePixelType m_InsideValue = 1;
}; // end of FixTopologyCarveOutside class

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkFixTopologyCarveOutside.hxx"
#endif

#endif // itkFixTopologyCarveOutside
