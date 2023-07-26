itk_module_test()

set(TopologyControlTests
  itkFixTopologyCarveOutsideTest.cxx
  )

CreateTestDriver(TopologyControl "${TopologyControl-Test_LIBRARIES}" "${TopologyControlTests}")

set(TEST_DATA_ROOT "${TopologyControl_SOURCE_DIR}/Data")

itk_add_test(NAME itkFixTopologyCarveOutsideTest
  COMMAND TopologyControlTestDriver
    --compare
    DATA{Baseline/itkFixTopologyCarveOutsideTestOutput.mha}
    ${ITK_TEST_OUTPUT_DIR}/itkFixTopologyCarveOutsideTestOutput.mha
  itkFixTopologyCarveOutsideTest
    ${ITK_TEST_OUTPUT_DIR}/itkFixTopologyCarveOutsideTestOutput.mha
)
