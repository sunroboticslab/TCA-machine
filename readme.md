<h1 align="center">Twisted and Coiled Actuator (TCA) Machine - V1 & V2</h1>

<p>Welcome to the Twisted and Coiled Actuator (TCA) Machine project repository! This project involves the design, manufacturing, and development of two versions of a machine used to fabricate TCAs for soft robotic applications. TCAs mimic biological actuators and are ideal for creating compliant, adaptive, and resilient robots. This repository contains the models, control software, documentation, and more for both versions of the TCA machine.</p>

<h2>Project Overview</h2>

<p>The TCA Machine is an automated fabrication system designed to create free stroke twisted and coiled actuators for use in soft robotic systems. These actuators provide high contraction ratios and are useful for many robotic applications, such as actuating tensegrity robots and shape-morphing quadrupedal robots.</p>

<p>There are two versions of the TCA machine available in this repository:</p>

<ul>
  <li><strong>Version 1</strong></li>
  <li><strong>Version 2</strong></li>
</ul>
<h3>Differences Between Version 1 and Version 2</h3>
<ul>
  <li>Version 2 has an increased width to improve reliability and provide more room for manipulation.</li>
  <li>The pulley in Version 2 is separated from the machine and suspended in the air to reduce friction and tension in the thread.</li>
  <li>A Stepper motor was removed in Version 2 as it was redundant.</li>
  <li>Transition from latch buttons to momentary buttons for improved control.</li>
  <li>Physical limit switches were added in Version 2 for enhanced safety and control.</li>
</ul>

<h2>Repository Structure</h2>

<h3>Version 1: TCA_Machine_Version_1/</h3>
<ul>
  <li><strong>01 SolidWorks</strong>
    <ul>
      <li><strong>01 Machine:</strong> Contains SolidWorks files for the mechanical design of the TCA machine.</li>
      <li><strong>02 Control_Box:</strong> SolidWorks files for the control box design, including details of the electronics.</li>
    </ul>
  </li>
  <li><strong>02 Arduino</strong>: Arduino code to control the motors and sensors of the machine, ensuring precise coiling and twisting.</li>
  <li><strong>03 Documentation</strong>: Contains documentation related to Version 1, which are listed below.
  <ul>
      <li><strong>Parts list TCA machine.xlsx:</strong> An Excel file listing all the parts used in Version 1 of the TCA machine.</li>
      <li><strong>TCA Machine.docx:</strong> Documentation describing the purpose and operation of the TCA machine.</li>
      <li><strong>TCA Manufacturing.pptx:</strong> A PowerPoint presentation that provides insights into the manufacturing process of TCAs.</li>
      <li><strong>motor layout.pptx:</strong> A presentation detailing the layout of the motors used in Version 1.</li>
    </ul>
  </li>
  <li><strong>04 Photos</strong>: A collection of photos showcasing the machine and its components.</li>
</ul>

<h3>Version 2: TCA_Machine_Version_2/</h3>
<ul>
  <li><strong>01 SolidWorks</strong>
    <ul>
      <li><strong>01 Machine</strong>: Updated SolidWorks files for the enhanced mechanical design.</li>
      <li><strong>02 Control_Box</strong>: Updated SolidWorks files for the improved control box design.</li>
    </ul>
  </li>
<li><strong>02 Arduino/TCACodeV2.0</strong>: Improved Arduino code for Version 2, featuring optimizations for smoother operation and enhanced control.</li>
<li><strong>03 Documentation</strong>: Updated user manuals and documentation for assembling and operating Version 2.
    <ul>
    <li><strong>Instructions_manualV2.pdf</strong>: A comprehensive user manual for Version 2, detailing Mechanical Construction, Electrical Construction, and how to use the machine.</li>
    <li><strong>Motor_Layout.pdf</strong>: A PDF document detailing the motor layout in Version 2.</li>
    <li><strong>TCA_Machine_Mech_BOM.xlsx</strong>: Bill of Materials for the mechanical components in Version 2.</li>
    <li><strong>TCA_Machine_Electric_BOM.xlsx</strong>: Bill of Materials for the electrical components in Version 2.</li>
    </ul>
  </li>
  <li><strong>04 Photos</strong>: Photos of Version 2 showcasing the new features and improvements.</li>
</ul>
