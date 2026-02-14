#include "../src/baseSimulator.h"

#include "gtest/gtest.h"

using T = double;

/** Case1:
 * 
 * operations 1 and 2
*/
TEST(DiffusiveMixing, case1) {

    // parameters
    T cWidth = 100e-6;
    T cHeight = 100e-6;
    T cLength = 2000e-6;
    T cLength2 = sqrt(2e-3*2e-3 + 1e-3*1e-3);
    T flowRate1 = 2e-10;
    T flowRate2 = 1e-10;
    T pecletNr1 = (flowRate1 / cHeight) / 1e-9; // (flowrate / height) / diffusivity
    T pecletNr2 = (flowRate2 / cHeight) / 1e-9; // (flowrate / height) / diffusivity
    // EXPECT_NEAR(pecletNr, 30.0, 1e-7);
    int resolution = 25;

    // create necessary objects
    std::vector<sim::FlowSectionInput<T>> constantFlowSections;
    std::vector<sim::FlowSectionInput<T>> constantFlowSectionsA;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections1;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections2;
    std::vector<sim::FlowSectionInput<T>> functionFlowSectionsA1;
    std::vector<sim::FlowSectionInput<T>> functionFlowSectionsA2;
    std::unordered_map<int, std::unique_ptr<sim::DiffusiveMixture<T>>> diffusiveMixtures;

    std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
    std::vector<T> zeroSegmentedResult = {0};

    sim::DiffusionMixingModel<T> diffusionMixingModelTest = sim::DiffusionMixingModel<T>();

    // start outflowChannel, end outFlowchannel, stretchFactor, start inflowChannel, concentration
    constantFlowSections.push_back({0.0, 0.5, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)});
    constantFlowSections.push_back({0.5, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)});
    // start outflowChannel, end outFlowchannel, stretchFactor, start inflowChannel, concentration
    constantFlowSectionsA.push_back({0.0, 0.5, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)});
    constantFlowSectionsA.push_back({0.5, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)});

    // perform analytical solution for constant input
    auto [fConstantA, segmentedResultConstantA, a_0_ConstantA] = diffusionMixingModelTest.getAnalyticalSolutionConstant(1e-3, cWidth, resolution, pecletNr1, constantFlowSectionsA);
    auto [fConstant0, segmentedResultConstant0, a_0_Constant0] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength, cWidth, resolution, pecletNr1, constantFlowSections);

    functionFlowSectionsA1.push_back({0.0, 1.0, 2.0, 0.0, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // bottom outflow channel
    functionFlowSectionsA2.push_back({0.0, 1.0, 2.0, 1.0, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // top outflow channel

    // perform analytical solution for function input
    auto [fFunctionA1, segmentedResultFunctionA1, a_0_FunctionA1] = diffusionMixingModelTest.getAnalyticalSolutionFunction(0.0, cWidth, resolution, pecletNr2, functionFlowSectionsA1, zeroFunction);
    auto [fFunctionA2, segmentedResultFunctionA2, a_0_FunctionA2] = diffusionMixingModelTest.getAnalyticalSolutionFunction(0.0, cWidth, resolution, pecletNr2, functionFlowSectionsA2, zeroFunction);

    functionFlowSections1.push_back({0.0, 1.0, 2.0, 0.0, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // bottom outflow channel
    functionFlowSections2.push_back({0.0, 1.0, 2.0, 1.0, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // top outflow channel

    // perform analytical solution for function input
    auto [fFunction1, segmentedResultFunction1, a_0_Function1] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength2, cWidth, resolution, pecletNr2, functionFlowSections1, zeroFunction);
    auto [fFunction2, segmentedResultFunction2, a_0_Function2] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength2, cWidth, resolution, pecletNr2, functionFlowSections2, zeroFunction);

    std::vector<T> results1 = {0.2256758334, -0.1400937832, 0.03671329178, -0.005362342632, 0.0005824036027, -4.876015006e-05, 2.926350265e-06};
    std::vector<T> results2 = {1.774324167, 0.1400937832, -0.03671329178, 0.005362342632, -0.0005824036027, 4.876015006e-05, -2.926350265e-06};

    EXPECT_NEAR(a_0_Function1, results1.at(0), 1e-9);
    EXPECT_NEAR(a_0_Function2, results2.at(0), 1e-9);

    for (size_t pos=1; pos<std::min(results1.size(), results2.size()); pos++) {
        EXPECT_NEAR(segmentedResultFunction1.at(pos-1), results1.at(pos), 1e-9);
        EXPECT_NEAR(segmentedResultFunction2.at(pos-1), results2.at(pos), 1e-9);
    }

    // std::cout << std::setprecision(10) << "coefficients 1: [" << a_0_Function1;
    // for (auto value : segmentedResultFunction1) {
    //     std::cout << ", " << value;
    // }
    // std::cout << "]" << std::endl;

    // std::cout << std::setprecision(10) << "coefficients 2: [" << a_0_Function2;
    // for (auto value : segmentedResultFunction2) {
    //     std::cout << ", " << value;
    // }
    // std::cout << "]" << std::endl;

}


/** Case2:
 * 
 * operations 3 and 4
*/
TEST(DiffusiveMixing, case2) {

    // parameters
    T cWidth = 100e-6;
    T cHeight = 100e-6;
    T cLength = 2000e-6;
    T cLength2 = sqrt(5e-6);
    // T cLength2 = 1e-6;
    T flowRate3 = 4e-10;
    T flowRate4 = 3.16267e-10;
    T flowRate5 = 4.41997e-11;
    T flowRate6 = 3.95334e-11;
    T pecletNr3 = (flowRate3 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr4 = (flowRate4 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr5 = (flowRate5 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr6 = (flowRate6 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    // EXPECT_NEAR(pecletNr, 30.0, 1e-7);
    int resolution = 50;

    // create necessary objects
    std::vector<sim::FlowSectionInput<T>> constantFlowSections;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections1;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections2;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections3;
    std::unordered_map<int, std::unique_ptr<sim::DiffusiveMixture<T>>> diffusiveMixtures;

    std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
    std::vector<T> zeroSegmentedResult = {0};

    sim::DiffusionMixingModel<T> diffusionMixingModelTest = sim::DiffusionMixingModel<T>();

    // start outflowChannel, end outFlowchannel, stretchFactor, start inflowChannel, concentration
    constantFlowSections.push_back({0.5, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // top inflow channel (double the width)
    constantFlowSections.push_back({0.25, 0.5, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    constantFlowSections.push_back({0.0, 0.25, 1.0, 0.0, 0.5, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel

    // perform analytical solution for constant input
    auto [fConstant0, segmentedResultConstant0, a_0_Constant0] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength, cWidth, resolution, pecletNr3, constantFlowSections);

    // functionFlowSections3.push_back({0.0, 1.0, 11.1317, 0.901167*11.1317, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // top outflow channel
    // functionFlowSections2.push_back({0.0, 1.0, 9.04984, 0.790667*9.04984, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // middle outflow channel
    // functionFlowSections1.push_back({0.0, 1.0, 0.5*2.52951, 0.0, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // bottom outflow channel

    functionFlowSections3.push_back({0.0, 1.0, 1.0/0.1, 0.901167, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // top outflow channel
    functionFlowSections2.push_back({0.0, 1.0, 1.0/0.11, 0.790667, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // middle outflow channel
    functionFlowSections1.push_back({0.0, 1.0, 1.0/0.79, 0.0, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // bottom outflow channel

    // perform analytical solution for function input
    auto [fFunctionC, segmentedResultFunctionC, a_0_FunctionC] = diffusionMixingModelTest.getAnalyticalSolutionFunction(0.0, 2*cWidth, resolution, pecletNr4, functionFlowSections1, zeroFunction);
    auto [fFunctionD, segmentedResultFunctionD, a_0_FunctionD] = diffusionMixingModelTest.getAnalyticalSolutionFunction(0.0, cWidth, resolution, pecletNr5, functionFlowSections2, zeroFunction);
    auto [fFunctionE, segmentedResultFunctionE, a_0_FunctionE] = diffusionMixingModelTest.getAnalyticalSolutionFunction(0.0, cWidth, resolution, pecletNr6, functionFlowSections3, zeroFunction);
    auto [fFunction1, segmentedResultFunction1, a_0_Function1] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength2, 2*cWidth, resolution, pecletNr4, functionFlowSections1, zeroFunction);
    auto [fFunction2, segmentedResultFunction2, a_0_Function2] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength, cWidth, resolution, pecletNr5, functionFlowSections2, zeroFunction);
    auto [fFunction3, segmentedResultFunction3, a_0_Function3] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength2, cWidth, resolution, pecletNr6, functionFlowSections3, zeroFunction);

    std::vector<T> results1 = {1.118113781, -0.1359068368, 0.009947944843, -0.000483563169, 2.325828538e-05, -6.340169129e-07, 9.394702258e-09};
    std::vector<T> results2 = {1.716126457, -0.0002302030334, -2.830038486e-11, -7.691691388e-21, -3.750019126e-35, -2.567501434e-52, -2.698636767e-74};
    std::vector<T> results3 = {1.780169756, -2.258037236e-05, -3.057304139e-13, -1.00006154e-25, -6.159218653e-43, -5.828095089e-65, -8.89415173e-92};
    std::vector<T> resultsC = {1.118113781, -0.1926489363, 0.04016378307, -0.01117328855, 0.006179917418, -0.003892544795, 0.002677899154};
    std::vector<T> resultsD = {1.716126457, -0.02002779484, -0.001621364682, -0.002196446739, -0.0004040051962, -0.0007898834035, -0.0001794484603};
    std::vector<T> resultsE = {1.780169756, -0.005999769181, -0.001523885785, -0.0006601729367, -0.0003801725161, -0.000237472167, -0.0001688995523};

    EXPECT_NEAR(a_0_Function1, results1.at(0), 1e-9);
    EXPECT_NEAR(a_0_Function2, results2.at(0), 1e-9);
    EXPECT_NEAR(a_0_Function3, results3.at(0), 1e-9);
    EXPECT_NEAR(a_0_FunctionC, resultsC.at(0), 1e-9);
    EXPECT_NEAR(a_0_FunctionD, resultsD.at(0), 1e-9);
    EXPECT_NEAR(a_0_FunctionE, resultsE.at(0), 1e-9);

    for (size_t pos=1; pos<std::min(results1.size(), results2.size()); pos++) {
        EXPECT_NEAR(segmentedResultFunction1.at(pos-1), results1.at(pos), 1e-9);
        EXPECT_NEAR(segmentedResultFunction2.at(pos-1), results2.at(pos), 1e-9);
        EXPECT_NEAR(segmentedResultFunction3.at(pos-1), results3.at(pos), 1e-9);
        EXPECT_NEAR(segmentedResultFunctionC.at(pos-1), resultsC.at(pos), 1e-9);
        EXPECT_NEAR(segmentedResultFunctionD.at(pos-1), resultsD.at(pos), 1e-9);
        EXPECT_NEAR(segmentedResultFunctionE.at(pos-1), resultsE.at(pos), 1e-9);
    }

    // std::cout << std::setprecision(10) << "coefficients 1: [" << a_0_Function1;
    // for (auto value : segmentedResultFunction1) {
    //     std::cout << ", " << value;
    // }
    // std::cout << "]" << std::endl;

    // std::cout << std::setprecision(10) << "coefficients 2: [" << a_0_Function2;
    // for (auto value : segmentedResultFunction2) {
    //     std::cout << ", " << value;
    // }
    // std::cout << "]" << std::endl;

    // std::cout << std::setprecision(10) << "coefficients 3: [" << a_0_Function3;
    // for (auto value : segmentedResultFunction3) {
    //     std::cout << ", " << value;
    // }
    // std::cout << "]" << std::endl;

    // std::cout << std::setprecision(10) << "coefficients C: [" << a_0_FunctionC;
    // for (auto value : segmentedResultFunctionC) {
    //     std::cout << ", " << value;
    // }
    // std::cout << "]" << std::endl;

    // std::cout << std::setprecision(10) << "coefficients D: [" << a_0_FunctionD;
    // for (auto value : segmentedResultFunctionD) {
    //     std::cout << ", " << value;
    // }
    // std::cout << "]" << std::endl;

    // std::cout << std::setprecision(10) << "coefficients E: [" << a_0_FunctionE;
    // for (auto value : segmentedResultFunctionE) {
    //     std::cout << ", " << value;
    // }
    // std::cout << "]" << std::endl;

}

/** Case3:
 * 
 * operation 5 twice
*/
TEST(DiffusiveMixing, case3) {

    // parameters
    T cWidth = 100e-6;
    T cHeight = 100e-6;
    T cLength = sqrt(1e-3*1e-3 + 1e-3*1e-3);
    [[maybe_unused]] T flowRate0 = 3.53553e-09;
    [[maybe_unused]] T flowRate1 = 3.53553e-09;
    [[maybe_unused]] T flowRate2 = 7.85674e-10;
    [[maybe_unused]] T flowRate3 = 6.28539e-09;
    [[maybe_unused]] T flowRate4 = 7.85674e-10;
    [[maybe_unused]] T flowRate5 = 6.28539e-09;
    [[maybe_unused]] T flowRate6 = 6.28539e-09;
    [[maybe_unused]] T flowRate7 = 7.85674e-10;
    T pecletNr2 = (flowRate2 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr3 = (flowRate3 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr6 = (flowRate6 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr7 = (flowRate7 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    // EXPECT_NEAR(pecletNr, 30.0, 1e-7);
    int resolution = 25;

    // create necessary objects
    std::vector<sim::FlowSectionInput<T>> constantFlowSections0;
    std::vector<sim::FlowSectionInput<T>> constantFlowSections1;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections1;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections2;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections3;
    std::unordered_map<int, std::unique_ptr<sim::DiffusiveMixture<T>>> diffusiveMixtures;

    std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
    std::vector<T> zeroSegmentedResult = {0};

    sim::DiffusionMixingModel<T> diffusionMixingModelTest = sim::DiffusionMixingModel<T>();

    // start outflowChannel, end outFlowchannel, stretchFactor, start inflowChannel, concentration
    // The constant flow sections that enter and flow through channels 3 and 5
    constantFlowSections0.push_back({0.0, 0.4375, 1.0, 0.222222, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
    constantFlowSections0.push_back({0.4375, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    auto [fConstant0, segmentedResultConstant0, a_0_Constant0] = diffusionMixingModelTest.getAnalyticalSolutionConstant(2*cLength, 2*cWidth, resolution, pecletNr3, constantFlowSections0);

    // The constant flow section of 0.0 concentration that enters through channels 2 and 4, this is ultimately just a function of 0.0 concentration
    constantFlowSections1.push_back({0.0, 1.0, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    auto [fConstant1, segmentedResultConstant1, a_0_Constant1] = diffusionMixingModelTest.getAnalyticalSolutionConstant(2*cLength, cWidth, resolution, pecletNr2, constantFlowSections1);

    // The top 0.125 part of channel 5 enters channel 7
    functionFlowSections1.push_back({0.0, 1.0, 4.0, 0.875, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // channel 7

    // The flow from channel 4 and the bottom 0.875 part of channel 5 enter channel 6
    functionFlowSections2.push_back({0.0, 0.125, 1.0, 0.0, T(0.0), fConstant1, segmentedResultConstant1, a_0_Constant1}); // channel 6
    functionFlowSections2.push_back({0.125, 1.0, 1.0, -0.125, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0});
    
    // perform analytical solution for function input
    auto [fFunction1, segmentedResultFunction1, a_0_Function1] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength, cWidth, resolution, pecletNr7, functionFlowSections1, zeroFunction);
    auto [fFunction2, segmentedResultFunction2, a_0_Function2] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength, 2*cWidth, resolution, pecletNr6, functionFlowSections2, zeroFunction);

    std::vector<T> results1 = {2.000000001, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    std::vector<T> results2 = {0.8749999999, -0.6039315827, 0.1066165206, 0.1307399654, -0.06604561768, -0.03076033584, 0.02954994509};

    EXPECT_NEAR(a_0_Function1, results1.at(0), 1e-9);
    EXPECT_NEAR(a_0_Function2, results2.at(0), 1e-9);

    for (size_t pos=1; pos<std::min(results1.size(), results2.size()); pos++) {
        EXPECT_NEAR(segmentedResultFunction1.at(pos-1), results1.at(pos), 1e-9);
        EXPECT_NEAR(segmentedResultFunction2.at(pos+23), results2.at(pos), 1e-9);
    }

    // std::cout << std::setprecision(10) << "coefficients 1: [" << a_0_Function1;
    // for (auto value : segmentedResultFunction1) {
    //     std::cout << ", " << value;
    // }
    // std::cout << "]" << std::endl;

    // std::cout << std::setprecision(10) << "coefficients 2: [" << a_0_Function2;
    // for (auto value : segmentedResultFunction2) {
    //     std::cout << ", " << value;
    // }
    // std::cout << "]" << std::endl;

}



/** Case4:
 * 
 * operations 1, 3 and 6
*/
TEST(DiffusiveMixing, case4) {

    // parameters
    T cWidth = 100e-6;
    T cHeight = 100e-6;
    [[maybe_unused]] T cLength1 = 2e-3;
    [[maybe_unused]] T cLength2 = sqrt(2e-3*2e-3 + 1e-3*1e-3);
    [[maybe_unused]] T cLength3 = sqrt(2e-3*2e-3 + 2e-3*2e-3);
    [[maybe_unused]] T flowRate0 = 3.23735e-10;
    [[maybe_unused]] T flowRate1 = 3.23735e-10;
    [[maybe_unused]] T flowRate2 = 3.23735e-10;
    [[maybe_unused]] T flowRate3 = 3.23735e-10;
    [[maybe_unused]] T flowRate4 = 6.47446e-10;
    [[maybe_unused]] T flowRate5 = 6.47446e-10;
    [[maybe_unused]] T flowRate6 = 1.29489e-09;
    [[maybe_unused]] T flowRate7 = 1.5942e-09;
    [[maybe_unused]] T flowRate8 = 1.5942e-09;
    [[maybe_unused]] T flowRate9 = 1.89351e-09;
    [[maybe_unused]] T flowRate10 = 6.07277e-10;
    [[maybe_unused]] T flowRate11 = 6.78956e-10;
    [[maybe_unused]] T flowRate12 = 6.07277e-10;
    [[maybe_unused]] T pecletNr0 = (flowRate0 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    [[maybe_unused]] T pecletNr4 = (flowRate4 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    [[maybe_unused]] T pecletNr6 = (flowRate6 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    [[maybe_unused]] T pecletNr7 = (flowRate7 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    [[maybe_unused]] T pecletNr9 = (flowRate9 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    [[maybe_unused]] T pecletNr10 = (flowRate10 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    [[maybe_unused]] T pecletNr11 = (flowRate11 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    // EXPECT_NEAR(pecletNr, 30.0, 1e-7);
    int resolution = 25;

    // create necessary objects
    std::vector<sim::FlowSectionInput<T>> constantFlowSections4;
    std::vector<sim::FlowSectionInput<T>> constantFlowSections5;
    std::vector<sim::FlowSectionInput<T>> constantFlowSections9;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections6;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections7;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections8;
    std::unordered_map<int, std::unique_ptr<sim::DiffusiveMixture<T>>> diffusiveMixtures;

    std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
    std::vector<T> zeroSegmentedResult = {0};

    sim::DiffusionMixingModel<T> diffusionMixingModelTest = sim::DiffusionMixingModel<T>();

    // start outflowChannel, end outFlowchannel, stretchFactor, start inflowChannel, concentration
    // The constant flow sections that enter channel 4
    constantFlowSections4.push_back({0.0, 0.5, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
    constantFlowSections4.push_back({0.5, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    auto [fConstant4, segmentedResultConstant4, a_0_Constant4] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength3, cWidth, resolution, pecletNr4, constantFlowSections4);

    // The constant flow sections that enter channel 5
    constantFlowSections5.push_back({0.0, 0.5, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    constantFlowSections5.push_back({0.5, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    auto [fConstant5, segmentedResultConstant5, a_0_Constant5] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength3, cWidth, resolution, pecletNr4, constantFlowSections5);

    // The function flow sections that enter channel 6
    functionFlowSections6.push_back({0.0, 0.5, 0.5, 0.0, T(0.0), fConstant4, segmentedResultConstant4, a_0_Constant4});
    functionFlowSections6.push_back({0.5, 1.0, 0.5, 0.0, T(0.0), fConstant5, segmentedResultConstant5, a_0_Constant5});
    auto [fFunction6, segmentedResultFunction6, a_0_Function6] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength1, cWidth, resolution, pecletNr6, functionFlowSections6, zeroFunction);

    // The constant flow sections that enter channel 9
    constantFlowSections9.push_back({0.0, 0.320715, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // channel 10
    constantFlowSections9.push_back({0.320715, 0.679285, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // channel 11
    constantFlowSections9.push_back({0.679285, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // channel 12
    auto [fFunction9, segmentedResultFunction9, a_0_Function9] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength1, cWidth, resolution, pecletNr9, constantFlowSections9);

    // The function flow sections that enter channel 7
    functionFlowSections7.push_back({0.0, 0.406126, 0.812252, 0.0, T(0.0), fFunction6, segmentedResultFunction6, a_0_Function6}); // channel 6
    functionFlowSections7.push_back({0.406126, 1.0, 1.18775, 0.5, T(0.0), fFunction9, segmentedResultFunction9, a_0_Function9});
    auto [fFunction7, segmentedResultFunction7, a_0_Function7] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength1, cWidth, resolution, pecletNr7, functionFlowSections7, zeroFunction);

    // The function flow sections that enter channel 8
    functionFlowSections8.push_back({0.0, 0.593874, 1.18775, 0.0, T(0.0), fFunction9, segmentedResultFunction9, a_0_Function9});
    functionFlowSections8.push_back({0.593874, 1.0, 0.812252, 0.5, T(0.0), fFunction6, segmentedResultFunction6, a_0_Function6});
    auto [fFunction8, segmentedResultFunction8, a_0_Function8] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength1, cWidth, resolution, pecletNr7, functionFlowSections8, zeroFunction);

    std::vector<T> results7 = {1.407907017, 0.245199986, 0.006914550309, -0.05244055709, -0.01749160537, 0.0004411017946, 0.0009288413936};
    std::vector<T> results8 = {1.283420113, 0.4586934067, 0.06845787687, -0.0117700664, 0.0003829626039, 3.990248299e-05, -0.0001994861841};

    EXPECT_NEAR(a_0_Function7, results7.at(0), 1e-9);
    EXPECT_NEAR(a_0_Function8, results8.at(0), 1e-9);

    for (size_t pos=1; pos<std::min(results7.size(), results8.size()); pos++) {
        EXPECT_NEAR(segmentedResultFunction7.at(pos-1), results7.at(pos), 1e-9);
        EXPECT_NEAR(segmentedResultFunction8.at(pos-1), results8.at(pos), 1e-9);
    }

    // std::cout << std::setprecision(10) << "coefficients 7: [" << a_0_Function7;
    // for (auto value : segmentedResultFunction7) {
    //     std::cout << ", " << value;
    // }
    // std::cout << "]" << std::endl;

    // std::cout << std::setprecision(10) << "coefficients 8: [" << a_0_Function8;
    // for (auto value : segmentedResultFunction8) {
    //     std::cout << ", " << value;
    // }
    // std::cout << "]" << std::endl;

}

/** 
 * Validation case against experimental results from 
 * Jeon, et. al., “Generation of solution and surface gradients using microfluidic systems,” 
 * Langmuir, vol. 16, no. 22, pp. 8311–8316, 2000.
 * 
 * Concentrations and flowrates are given for 10 mmm/s inlet flow rate
*/
TEST(DiffusiveMixing, JeonEtAl_10mms) {

    //   0 1 2  3 4 5  6 7 8
    //   | | |  | | |  | | |
    //    \|/    \|/    \|/
    //     9      10     11
    //      \     |     /
    //        \   |   /
    //          \ | /
    //            12
    //            |


    // parameters
    [[maybe_unused]] auto cWidth = 50e-6;
    [[maybe_unused]] auto cHeight = 100e-6;
    [[maybe_unused]] auto cLength = 570e-10;  // Channel length

    // Concentrations (from Jeon et al. read out from simulations)
    T c0 = 0.0;
    T c1 = 0.1784;
    T c2 = 0.3991;
    T c3 = 0.5778;
    T c4 = 0.6459;
    T c5 = 0.5778;
    T c6 = 0.3991;
    T c7 = 0.1784;
    T c8 = 0.0;

    T diffusivity = 5e-10; // m^2/s

    [[maybe_unused]] T flowRate0 = 1.634586e-11; // channel 87
    [[maybe_unused]] T flowRate1 = 1.62243e-11; // channel 88
    [[maybe_unused]] T flowRate2 = 1.676514e-11; // channel 89

    [[maybe_unused]] T flowRate3 = 1.707595e-11; // channel 90
    [[maybe_unused]] T flowRate4 = 1.717750e-11; // channel 91
    [[maybe_unused]] T flowRate5 = 1.707595e-11; // channel 92

    [[maybe_unused]] T flowRate6 = 1.676514e-11; // channel 93
    [[maybe_unused]] T flowRate7 = 1.62243e-11; // channel 94
    [[maybe_unused]] T flowRate8 = 1.634586e-11; // channel 95

    // Flowrates (calculated using 1D simulation)
    [[maybe_unused]] T flowRate9 = 4.93353e-11; // channel 96
    [[maybe_unused]] T flowRate10 = 5.13294e-11; // channel 97
    [[maybe_unused]] T flowRate11 = 4.93353e-11; // channel 98
    [[maybe_unused]] T flowRate12 = 1.5e-10; // channel 99

    T pecletNr9 = (flowRate9 / cHeight) / diffusivity; // (flowrate / height) / diffusivity
    T pecletNr10 = (flowRate10 / cHeight) / diffusivity; // (flowrate / height) / diffusivity
    T pecletNr11 = (flowRate11 / cHeight) / diffusivity; // (flowrate / height) / diffusivity
    T pecletNr12 = (flowRate12 / cHeight) / diffusivity; // (flowrate / height) / diffusivity

    int resolution = 100;

    // create necessary objects
    std::vector<sim::FlowSectionInput<T>> constantFlowSections9;
    std::vector<sim::FlowSectionInput<T>> constantFlowSections10;
    std::vector<sim::FlowSectionInput<T>> constantFlowSections11;

    std::vector<sim::FlowSectionInput<T>> functionFlowSections12;

    std::unordered_map<int, std::unique_ptr<sim::DiffusiveMixture<T>>> diffusiveMixtures;

    std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
    std::vector<T> zeroSegmentedResult = {0};

    sim::DiffusionMixingModel<T> diffusionMixingModelTest = sim::DiffusionMixingModel<T>();

    // inflow into channel 9 (aq. to channel 96 in main.cpp)
    constantFlowSections9.push_back({0.0, 0.331322, 0.331322, 0.0, c0, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 0
    constantFlowSections9.push_back({0.331322, 0.660180, 0.328858, 0.0, c1, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 1
    constantFlowSections9.push_back({0.66018, 1.0, 0.339820, 0.0, c2, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 2
    // inflow into channel 10 (aq. to channel 97 in main.cpp)
    constantFlowSections10.push_back({0.0, 0.332674, 0.332674, 0.0, c3, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 3
    constantFlowSections10.push_back({0.332674, 0.667326, 0.334652, 0.0, c4, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 4
    constantFlowSections10.push_back({0.667326, 1.0, 0.332674, 0.0, c5, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 5
    // inflow into channel 11 (aq. to channel 98 in main.cpp)
    constantFlowSections11.push_back({0.0, 0.339820, 0.339820, 0.0, c6, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 6
    constantFlowSections11.push_back({0.339820, 0.6686780, 0.328858, 0.0, c7, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 7
    constantFlowSections11.push_back({0.668678, 1.0, 0.331322, 0.0, c8, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 8
    
    auto [fConstant9, segmentedResultConstant9, a_0_Constant9] = diffusionMixingModelTest.getAnalyticalSolutionConstant(0.0, 3*cWidth, resolution, pecletNr9, constantFlowSections9);
    auto [fConstant10, segmentedResultConstant10, a_0_Constant10] = diffusionMixingModelTest.getAnalyticalSolutionConstant(0.0, 3*cWidth, resolution, pecletNr10, constantFlowSections10);
    auto [fConstant11, segmentedResultConstant11, a_0_Constant11] = diffusionMixingModelTest.getAnalyticalSolutionConstant(0.0, 3*cWidth, resolution, pecletNr11, constantFlowSections11);

    // Flow into channel 10 generated from channel 8
    functionFlowSections12.push_back({0.0, 0.328902, 0.328902, 0.0, T(0.0), fConstant9, segmentedResultConstant9, a_0_Constant9});
    functionFlowSections12.push_back({0.328902, 0.671098, 0.342196, 0.0, T(0.0), fConstant10, segmentedResultConstant10, a_0_Constant10});
    functionFlowSections12.push_back({0.671098, 1.0,  0.328902, 0.0, T(0.0), fConstant11, segmentedResultConstant11, a_0_Constant11});
        
    auto [fFunction12, segmentedResultFunction12, a_0_Function12] = diffusionMixingModelTest.getAnalyticalSolutionFunction(500e-6, 18*cWidth, resolution, pecletNr12, functionFlowSections12, zeroFunction);

    // perform analytical solution for function input

    // generate resulting csv files
    std::ofstream outputFile0;
    std::ofstream outputFile1;
    std::ofstream outputFile2;
    std::ofstream outputFile3;


    //
    outputFile0.open("Jeon_out0_10mms.csv");  // Channel 9 output
    outputFile1.open("Jeon_out1_10mms.csv");  // Channel 10 output
    outputFile2.open("Jeon_out2_10mms.csv");  // Channel 11 output
    outputFile3.open("Jeon_out3_10mms.csv");  // Channel 12 output

    
    outputFile0 << "x,f(x)\n";
    outputFile1 << "x,f(x)\n";
    outputFile2 << "x,f(x)\n";
    outputFile3 << "x,f(x)\n";

    int numValues = 101;
    double xStart = 0.0, xEnd = 1.0;
    double range = xEnd - xStart;
    double step = range / (numValues - 1);

    for (int i = 0; i < numValues; ++i) {
        T x = xStart + i * step;
        //y0 = fConstant0(x);
        T y0 = fConstant9(x);
        T y1 = fConstant10(x);
        T y2 = fConstant11(x);
        T y3 = fFunction12(x);
        outputFile0 << std::setprecision(4) << x << "," << y0 << "\n"; 
        outputFile1 << std::setprecision(4) << x << "," << y1 << "\n"; 
        outputFile2 << std::setprecision(4) << x << "," << y2 << "\n"; 
        outputFile3 << std::setprecision(4) << x << "," << y3 << "\n";
    }
    // Close the file
    outputFile0.close();
    outputFile1.close();
    outputFile2.close();
    outputFile3.close();

}


/** Case3:
 * 
 * operation 5 four times
*/
TEST(DiffusiveMixing, caseJeonEtAl) {
    //   0 1 2  3 4 5  6 7 8
    //   | | |  | | |  | | |
    //    \|/    \|/    \|/
    //     9      10     11
    //      \     |     /
    //        \   |   /
    //          \ | /
    //            12
    //            |


    // parameters
    [[maybe_unused]] auto cWidth = 50e-6;
    [[maybe_unused]] auto cHeight = 100e-6;
    [[maybe_unused]] auto cLength = 570e-10;  // Channel length

    // Concentrations (from Jeon et al. read out from simulations)
    T c0 = 0.0;
    T c1 = 0.1784;
    T c2 = 0.3991;
    T c3 = 0.5778;
    T c4 = 0.6459;
    T c5 = 0.5778;
    T c6 = 0.3991;
    T c7 = 0.1784;
    T c8 = 0.0;

    /**
     * 1 mm/s
    */
    // T c0 = 0.0;
    // T c1 = 0.2167;
    // T c2 = 0.4115;
    // T c3 = 0.5513;
    // T c4 = 0.6021;
    // T c5 = 0.5513;
    // T c6 = 0.4115;
    // T c7 = 0.2267;
    // T c8 = 0.0;

    T diffusivity = 5e-10; // m^2/s

    // Flowrates (calculated using 1D simulation)

    [[maybe_unused]] T flowRate0 = 5.44862e-12; // channel 87
    [[maybe_unused]] T flowRate1 = 5.4081e-12; // channel 88
    [[maybe_unused]] T flowRate2 = 5.58838e-12; // channel 89
    [[maybe_unused]] T flowRate3 = 5.69198e-12; // channel 90
    [[maybe_unused]] T flowRate4 = 5.72583e-12; // channel 91
    [[maybe_unused]] T flowRate5 = 5.69198e-12; // channel 92
    [[maybe_unused]] T flowRate6 = 5.58838e-12; // channel 93
    [[maybe_unused]] T flowRate7 = 5.4081e-12; // channel 94
    [[maybe_unused]] T flowRate8 = 5.44862e-12; // channel 95

    [[maybe_unused]] T flowRate9 = 4.93353e-11; // channel 96
    [[maybe_unused]] T flowRate10 = 5.13294e-11; // channel 97
    [[maybe_unused]] T flowRate11 = 4.93353e-11; // channel 98
    [[maybe_unused]] T flowRate12 = 1.5e-10; // channel 99

    /**
     * 1mm/s
    */
    // T flowRate0 = 5.44862e-13; // channel 87
    // T flowRate1 = 5.4081e-13; // channel 88
    // T flowRate2 = 5.58838e-13; // channel 89
    // T flowRate3 = 5.69198e-13; // channel 90
    // T flowRate4 = 5.72583e-13; // channel 91
    // T flowRate5 = 5.69198e-13; // channel 92
    // T flowRate6 = 5.58838e-13; // channel 93
    // T flowRate7 = 5.4081e-13; // channel 94
    // T flowRate8 = 5.44862e-13; // channel 95

    // T flowRate9 = 1.64451e-12; // channel 96
    // T flowRate10 = 1.71098e-12; // channel 97
    // T flowRate11 = 1.64451e-12; // channel 98
    // T flowRate12 = 5e-12; // channel 99


    T pecletNr9 = (flowRate9 / cHeight) / diffusivity; // (flowrate / height) / diffusivity
    T pecletNr10 = (flowRate10 / cHeight) / diffusivity; // (flowrate / height) / diffusivity
    T pecletNr11 = (flowRate11 / cHeight) / diffusivity; // (flowrate / height) / diffusivity
    T pecletNr12 = (flowRate12 / cHeight) / diffusivity; // (flowrate / height) / diffusivity
    // EXPECT_NEAR(pecletNr, 30.0, 1e-7);
    int resolution = 100;

    // create necessary objects
    std::vector<sim::FlowSectionInput<T>> constantFlowSections9;
    std::vector<sim::FlowSectionInput<T>> constantFlowSections10;
    std::vector<sim::FlowSectionInput<T>> constantFlowSections11;

    std::vector<sim::FlowSectionInput<T>> functionFlowSections12;

    std::unordered_map<int, std::unique_ptr<sim::DiffusiveMixture<T>>> diffusiveMixtures;

    std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
    std::vector<T> zeroSegmentedResult = {0};

    sim::DiffusionMixingModel<T> diffusionMixingModelTest = sim::DiffusionMixingModel<T>();

    // inflow into channel 9 (aq. to channel 96 in main.cpp)
    constantFlowSections9.push_back({0.0, 0.331277, 1.0, 0, c0, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
    constantFlowSections9.push_back({0.331277, 0.660117, 0.328839, -0.331277, c1, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    constantFlowSections9.push_back({0.660117, 1.0, 1.0, -0.660117, c2, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
    // inflow into channel 10 (aq. to channel 97 in main.cpp)
    constantFlowSections10.push_back({0.0, 0.332338, 0.332338, 0.0, c3, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    constantFlowSections10.push_back({0.332338, 0.666925, 0.334588, -0.332338, c4, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
    constantFlowSections10.push_back({0.666925, 1.0, 0.333075, -0.666925, c5, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    // inflow into channel 11 (aq. to channel 98 in main.cpp)
    constantFlowSections11.push_back({0.0, 0.339695, 1.0, 0.0, c6, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
    constantFlowSections11.push_back({0.339695, 0.668593, 0.328898, -0.339695, c7, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    constantFlowSections11.push_back({0.668593, 1.0, 1.0, -0.668593, c8, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
    
    auto [fConstant9, segmentedResultConstant9, a_0_Constant9] = diffusionMixingModelTest.getAnalyticalSolutionConstant(0.0, 3*cWidth, resolution, pecletNr9, constantFlowSections9);
    auto [fConstant10, segmentedResultConstant10, a_0_Constant10] = diffusionMixingModelTest.getAnalyticalSolutionConstant(0.0, 3*cWidth, resolution, pecletNr10, constantFlowSections10);
    auto [fConstant11, segmentedResultConstant11, a_0_Constant11] = diffusionMixingModelTest.getAnalyticalSolutionConstant(0.0, 3*cWidth, resolution, pecletNr11, constantFlowSections11);

    // Flow into channel 10 generated from channel 8
    functionFlowSections12.push_back({0.0, 0.328231, 0.328231, 0.0, T(0.0), fConstant9, segmentedResultConstant9, a_0_Constant9});
    functionFlowSections12.push_back({0.328231, 0.670296, 0.342066, 0.0, T(0.0), fConstant10, segmentedResultConstant10, a_0_Constant10});
    functionFlowSections12.push_back({0.670296, 1.0,  0.329704, 0.0, T(0.0), fConstant11, segmentedResultConstant11, a_0_Constant11});
        
    auto [fFunction12, segmentedResultFunction12, a_0_Function12] = diffusionMixingModelTest.getAnalyticalSolutionFunction(500e-6, 18*cWidth, resolution, pecletNr12, functionFlowSections12, zeroFunction);

    // perform analytical solution for function input

    // generate resulting csv files
    std::ofstream outputFile0;
    std::ofstream outputFile1;
    std::ofstream outputFile2;
    std::ofstream outputFile3;


    //
    outputFile0.open("Jeon_out0.csv");  // Channel 9 output
    outputFile1.open("Jeon_out1.csv");  // Channel 10 output
    outputFile2.open("Jeon_out2.csv");  // Channel 11 output
    outputFile3.open("Jeon_out3.csv");  // Channel 12 output

    
    outputFile0 << "x,f(x)\n";
    outputFile1 << "x,f(x)\n";
    outputFile2 << "x,f(x)\n";
    outputFile3 << "x,f(x)\n";

    int numValues = 101;
    double xStart = 0.0, xEnd = 1.0;
    double range = xEnd - xStart;
    double step = range / (numValues - 1);

    for (int i = 0; i < numValues; ++i) {
        T x = xStart + i * step;
        //y0 = fConstant0(x);
        T y0 = fConstant9(x);
        T y1 = fConstant10(x);
        T y2 = fConstant11(x);
        T y3 = fFunction12(x);
        outputFile0 << std::setprecision(4) << x << "," << y0 << "\n"; 
        outputFile1 << std::setprecision(4) << x << "," << y1 << "\n"; 
        outputFile2 << std::setprecision(4) << x << "," << y2 << "\n"; 
        outputFile3 << std::setprecision(4) << x << "," << y3 << "\n";
    }
    // Close the file
    outputFile0.close();
    outputFile1.close();
    outputFile2.close();
    outputFile3.close();

}
