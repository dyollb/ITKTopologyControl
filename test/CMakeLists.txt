itk_module_test()

set(TopologyControlTests
  itkFixTopologyCarveOutsideTest.cxx
  itkFixTopologyCarveInsideTest.cxx
  )

CreateTestDriver(TopologyControl "${TopologyControl-Test_LIBRARIES}" "${TopologyControlTests}")

set(TEST_DATA_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

itk_add_test(NAME itkFixTopologyCarveOutsideTest
  COMMAND TopologyControlTestDriver
    --compare
    DATA{Baseline/itkFixTopologyCarveOutsideTestOutput.mha}
    ${ITK_TEST_OUTPUT_DIR}/itkFixTopologyCarveOutsideTestOutput.mha
  itkFixTopologyCarveOutsideTest
    ${ITK_TEST_OUTPUT_DIR}/itkFixTopologyCarveOutsideTestOutput.mha
)

itk_add_test(NAME itkFixTopologyCarveInsideTest
  COMMAND TopologyControlTestDriver
    --compare
    DATA{Baseline/itkFixTopologyCarveInsideTestOutput.mha}
    ${ITK_TEST_OUTPUT_DIR}/itkFixTopologyCarveInsideTestOutput.mha
    itkFixTopologyCarveInsideTest
    ${ITK_TEST_OUTPUT_DIR}/itkFixTopologyCarveInsideTestOutput.mha
)
