<?xml version="1.0" encoding="utf-8" ?>
<!-- Definitions of hints for how to log model operator items -->
<SMTK_AttributeSystem Version="2">
  <Definitions>
    <AttDef Type="hint">
      <BriefDescription>
        The base definition for all hints.
      </BriefDescription>
    </AttDef>

    <AttDef Type="find-by-name" BaseType="hint">
      <ItemDefinitions>
        <String Name="name" NumberOfRequiredValues="1" Extensible="true">
          <BriefDescription>
            The names of model entities to select.
          </BriefDescription>
        </String>
        <!-- might want to restrict to a specific type as well... -->
      </ItemDefinitions>
    </AttDef>

    <AttDef Type="pick-along-line" BaseType="hint">
      <ItemDefinitions>
        <Double Name="base point" NumberOfRequiredValues="3">
          <BriefDescription>
            A point on the line; the model entities closest to this point
            in the direction of the ray are given priority by default.
          </BriefDescription>
        </Double>
        <Double Name="direction" NumberOfRequiredValues="3">
          <BriefDescription>
            The direction of the line along which to pick items.
          </BriefDescription>
        </Double>
        <Int Name="i-th" NumberOfRequiredValues="1" Extensible="true">
          <DefaultValue>0</DefaultValue>
          <BriefDescription>
            The index of the item(s) to pick.
            Items are ordered starting at 0 with those closest to the base point first.
            The default is to choose only item 0.
          </BriefDescription>
        </Int>
      </ItemDefinitions>
    </AttDef>

  </Definitions>
</SMTK_AttributeSystem>
