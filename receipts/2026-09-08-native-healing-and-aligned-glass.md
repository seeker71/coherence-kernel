# Native healing and aligned Glass

Codex, 2026-09-08. Urs asked to remove the Python healing detour and align the
views. The healing workflow's twelve Python files have been replaced by Form
cells. Older repository SDK and CI tooling are outside this migration.

All twelve Glass views keep their footer at the final viewport row. Evidence
stays above it in the views that carry an evidence panel. Short bodies fill
their allocated rows; one-, two-, and three-row windows retain the chrome that
fits. Composite rows clip to the viewport at UTF-8 boundaries while retaining
segment style and evidence identity. The atlas had emitted 54 columns in a
40-column window; that wrapping path is closed.

The vertical layout panel returned **4095/4095** across all twelve views at six
sizes. The existing UI band returned 4294967295; the render band returned 4095.
A browser review of the production-composed layout fixtures found all twelve
footers at row 24 in 80×24 frames and row 10 in 40×10 frames. These were layout
fixtures, not live telemetry or a live Terminal capture.

Form now supervises argv children, streams private output to retained files,
parses metadata, records correlated choices, and accounts for elapsed intervals.
The native process witness passed nine checks, including timeout, cancellation,
detached descendants, metadata schema rejection, private-output separation,
and stream visibility before process completion. The snapshot witness passed
seven checks covering hash identity, stale input, changed checkers, restoration,
candidate races, guarded replacement, and snapshot release.

The native six-case evaluation measured three structural repairs, two unresolved
semantic cases, and one preserved correct control. Remote calls were zero;
training was excluded and live repair memory remained unchanged. Evidence:
`.form-heal/eval-native-23555-83216440-0/summary.json`.

The first native learning attempts were refused. They exposed a Q4 embedding
decoded through the Q8 kernel, an incorrect RMS epsilon index in the fitting
reader, and missing rejection of nonfinite device floats. The decoder now
follows the tensor type across all four embedding call paths. A real Metal
Q4_K row-offset witness returned 31, and the nonfinite-value band returned 15.
The dense Qwen pipeline band returned 2147483647.

After those repairs, two real native fitting rounds completed in **38,926 ms**
and **34,014 ms**. Each changed the B tensor; the second parent hash matched the
first candidate hash. Both full base-file checks matched. Loss values, scaled
by 10^12, moved from 1458.026588 to 820.139956 and from 1509.922409 to 849.331355.
Each round observed one fixed-probe forward token and fit 5,120 activation-space
elements. This is an outcome-embedding squared-error objective, not next-token
loss, general repair skill, or a serving promotion. Evidence:
`.form-heal/learning-native-witness-36394-83802333-0/learning.jsonl`.

The local HTTP client uses the documented Ollama streaming response and final
token counters ([API contract](https://docs.ollama.com/api/generate)). It does
not infer token counts from response chunks. Remote-provider token accounting,
resident context recycling, repair-quality learning, and a demonstrated
hardware floor remain open. The full base scans and model admission are real
costs; a fitting update's success does not make those costs disappear.

The Metal attention check again printed its attended-position observation and
then 255, exiting zero. The earlier stop was a wrapper-contract defect. No
expected result or historical failure was rewritten to create a pass.

The structural gate first refused 75 retained native launch/inventory/link
scripts. Their narrow OS carrier role is now explicit under the healing spool;
arbitrary shell and Python remain unclassified. Evidence was retained. Empty
process reports no longer claim reconciled timing, and repeat local model
inventories retain distinct traces.

The exchange stayed alive by turning each refusal into a smaller observable
check. The surprising teaching was that two matching decoder paths can share
the same type error. The uncomfortable failed training attempt yielded a real
decoder correction and an explicit boundary on what the new learning proves.
