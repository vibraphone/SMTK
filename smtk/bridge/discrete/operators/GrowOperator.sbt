<?xml version="1.0" encoding="utf-8" ?>
<!-- Description of the CMB Discrete Model "grow" Operator -->
<SMTK_AttributeSystem Version="2">
  <Definitions>
    <!-- Operator -->
    <AttDef Type="grow" BaseType="operator">
      <ItemDefinitions>
        <ModelEntity Name="model" NumberOfRequiredValues="1">
          <MembershipMask>model</MembershipMask>
        </ModelEntity>
        <MeshSelection Name="selection" ModelEntityRef="model">
          <MembershipMask>face</MembershipMask>
        </MeshSelection>
        <Double Name="feature angle" NumberOfRequiredValues="1">
          <DefaultValue>30.0</DefaultValue>
          <RangeInfo>
            <Min Inclusive="true">0.</Min>
            <Max Inclusive="true">180.</Max>
          </RangeInfo>
        </Double>
      </ItemDefinitions>
    </AttDef>
    <!-- Result -->
    <AttDef Type="result(grow)" BaseType="result">
      <ItemDefinitions>
        <MeshSelection Name="selection">
        </MeshSelection>
      </ItemDefinitions>
    </AttDef>
  </Definitions>
</SMTK_AttributeSystem>
