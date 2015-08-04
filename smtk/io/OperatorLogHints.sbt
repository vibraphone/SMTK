<?xml version="1.0" encoding="utf-8" ?>
<!-- Definitions of hints for how to log model operator items -->
<SMTK_AttributeSystem Version="2">
  <Definitions>
    <AttDef Type="log hint">
      <BriefDescription>
        The base definition for all logging hints.
      </BriefDescription>
      <ItemDefinitions>
        <String Name="context" NumberOfRequiredValues="1">
          <BriefDescription>
            An application-assigned name identifying the attribute or
            context we wish to provide hints for.
            This may be any string the application deems useful, but is
            generally not the source of the hints; usually it is the
            name of the attribute(s) being hinted.
            Definitions that inherit "log hint" provide a way to specify
            the hint itself while this base definition provides a
            common way for applications to determine which items a hint
            might apply to.
          </BriefDescription>
        </String>
        <String Name="item name" Optional="true" Extensible="true">
          <BriefDescription>
            The name of the attribute item holding the value we wish to hint.
            This may also be used as a regular expression, depending on the context.
            As with the context, this is a freeform field that may be used
            differently depending on the application, context, and hint type.
            Note that entries of this member should come in pairs; the
            first entry specifies the path of an item and the second specifies
            the separator character(s) used to split the path. If
            only 1 value is provided, the separator is assumed to be a
            forward-slash ("/").
          </BriefDescription>
        </String>
        <Int Name="item index" Optional="true" Extensible="true">
          <BriefDescription>
            Some items accept multiple values (for example, a DoubleItem
            representing point coordinates might accept 3 values).
            The index refers to the particular value(s) being hinted.

            Each value of an item may have zero or more hints which apply to it.
          </BriefDescription>
        </Int>
      </ItemDefinitions>
    </AttDef>

    <AttDef Type="previous-result" BaseType="log hint">
      <ItemDefinitions>
        <Int Name="result index" NumberOfRequiredValues="1">
          <DefaultValue>-1</DefaultValue>
          <BriefDescription>
            The index of the operator result holding the value we wish to use.
            Items are ordered starting at 0 for the first operation and increasing
            from there, but negative numbers may be used to indicate counting
            backwards from the most recent result (-1).

            Note that when using negative numbers, the application is responsible
            for updating result indices as each result is added to the system.
            Otherwise, pre-existing hints with a result index of -1 would
            incorrectly refer to the new result.

            The index is across all operations being logged (i.e., all operations
            on a given model manager).
          </BriefDescription>
        </Int>
        <String Name="result group" NumberOfRequiredValues="1">
          <BriefDescription>
            The name of the attribute item holding the value we wish to reuse.
            Usually this will be either "created" or "modified."
          </BriefDescription>
        </String>
        <Int Name="entries" Optional="true" Extensible="true">
          <BriefDescription>
            The indices of the value(s) to pick from the item named by "result group".
            Items are ordered starting at 0.
            If unspecified, the entire list named by "result group" is used.
          </BriefDescription>
        </Int>
      </ItemDefinitions>
    </AttDef>

    <AttDef Type="find-by-name" BaseType="log hint">
      <ItemDefinitions>
        <String Name="name" NumberOfRequiredValues="1" Extensible="true">
          <BriefDescription>
            The names of model entities to select.
          </BriefDescription>
        </String>
        <Int Name="type filter" Optional="true">
          <BriefDescription>
            A bitmask used to specify the types of entities to accept.
            This is useful if names are only unique among entities of a given type.
          </BriefDescription>
        </Int>
        <!-- might want to restrict to a specific type as well... -->
      </ItemDefinitions>
    </AttDef>

    <AttDef Type="pick-along-line" BaseType="log hint">
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