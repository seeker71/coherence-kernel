# Observed healing run

Wall intervals include waiting; they are not CPU or hardware-floor measurements.

- baseline-preflight: 217 ms; state=finished; reason=completed; exit=0
  intervals=[["process-launch",0,0,0],["process-spawn-begin",0,80,80],["process-child-observed",80,206,126],["process-reaped",206,216,10],["process-cleanup-observed",216,217,1]]
  token_flows=[]
- baseline: 167 ms; state=finished; reason=completed; exit=0
  intervals=[["process-launch",0,0,0],["process-spawn-begin",0,81,81],["process-child-observed",81,156,75],["process-reaped",156,166,10],["process-cleanup-observed",166,167,1]]
  token_flows=[]
- check-eval-model-preflight: 709 ms; state=finished; reason=completed; exit=0
  intervals=[["process-launch",0,0,0],["process-spawn-begin",0,71,71],["process-child-observed",71,701,630],["process-reaped",701,708,7],["process-cleanup-observed",708,709,1]]
  token_flows=[]
- check-eval-model: 127 ms; state=finished; reason=completed; exit=0
  intervals=[["process-launch",0,0,0],["process-spawn-begin",0,76,76],["process-child-observed",76,120,44],["process-reaped",120,127,7],["process-cleanup-observed",127,127,0]]
  token_flows=[]
- eval-model: 553882 ms; state=finished; reason=completed; exit=0
  intervals=[["process-launch",0,0,0],["process-spawn-begin",0,88,88],["process-child-observed",88,421,333],["model-runner-admission-door-2",421,421,0],["model-session-seal-begin",421,14218,13797],["model-session-seal-complete",14218,14218,0],["model-session-header-begin",14218,14273,55],["model-session-header-complete",14273,14273,0],["model-session-tokenizer-crystal-begin",14273,14273,0],["model-session-tokenizer-crystal-complete",14273,14273,0],["model-session-prompt-cursor-begin",14273,17832,3559],["model-session-prompt-cursor-complete",17832,17832,0],["model-session-prompt-count-begin",17832,17832,0],["model-session-prompt-count-complete",17832,17832,0],["model-session-prompt-positions-766",17832,17832,0],["model-session-prompt-cursor-reopen-begin",17832,17832,0],["model-session-prompt-cursor-reopen-complete",17832,17832,0],["model-session-open-prefill-begin",17832,17832,0],["model-session-prefill-span-64",17832,17832,0],["model-session-context-open-begin",17832,17832,0],["model-artifact-admission-begin",17832,18945,1113],["model-artifact-admission-ready",18945,18945,0],["model-geometry-begin",18945,18945,0],["model-geometry-ready",18945,18945,0],["model-pipeline-jit-begin",18945,19379,434],["model-pipeline-jit-ready",19379,19379,0],["model-scratch-begin",19379,19379,0],["model-scratch-ready",19379,19379,0],["model-layer-map-begin",19379,32285,12906],["model-layer-map-ready",32285,32285,0],["model-tensor-view-begin",32285,32285,0],["model-tensor-view-ready",32285,32285,0],["model-open-ready",32285,32285,0],["model-session-context-open-ready",32285,32285,0],["model-session-state-begin",32285,32408,123],["model-session-state-ready",32408,32408,0],["model-session-prefill-begin",32408,59372,26964],["model-prefill-positions-64",59372,69311,9939],["model-prefill-positions-128",69311,79088,9777],["model-prefill-positions-192",79088,89110,10022],["model-prefill-positions-256",89110,98910,9800],["model-prefill-positions-320",98910,108739,9829],["model-prefill-positions-384",108739,118624,9885],["model-prefill-positions-448",118624,128759,10135],["model-prefill-positions-512",128759,137869,9110],["model-prefill-positions-576",137869,147838,9969],["model-prefill-positions-640",147838,158739,10901],["model-prefill-positions-704",158739,169485,10746],["model-prefill-positions-766",169485,169683,198],["model-session-prefill-ready",169683,169683,0],["model-session-open-prefill-complete",169683,553872,384189],["process-reaped",553872,553877,5],["process-cleanup-observed",553877,553882,5]]
  token_flows=[{"phase":"eos","route":"native-fkwu-metal","model":"qwen38-q4","model_path":"/Users/ursmuff/models/qwen38-27b/Qwen3.8-27B-Q4_K_M.gguf","base_seal_sha256":"14dd37b54fb4ac3240dde91549a6ac20ed00cf2716d6c542553f40662ead55b3","adapter_path":"","adapter_sha256":"","adapter_state":"base-only","input_tokens":766,"output_tokens":113,"position":879,"stamp_ms":1789026542504,"elapsed_ms":553789}]
- freshness: 160 ms; state=finished; reason=completed; exit=0
  intervals=[["process-launch",0,0,0],["process-spawn-begin",0,75,75],["process-child-observed",75,146,71],["process-reaped",146,159,13],["process-cleanup-observed",159,160,1]]
  token_flows=[]
- ground: 442 ms; state=finished; reason=completed; exit=0
  intervals=[["process-launch",0,0,0],["process-spawn-begin",0,92,92],["process-child-observed",92,433,341],["process-reaped",433,441,8],["process-cleanup-observed",441,442,1]]
  token_flows=[]
- inventory: 3476 ms; state=finished; reason=completed; exit=0
  intervals=[["process-launch",0,0,0],["process-spawn-begin",0,100,100],["process-child-observed",100,3466,3366],["process-reaped",3466,3475,9],["process-cleanup-observed",3475,3476,1]]
  token_flows=[]

## Recorded choices

- ground [1789025983223] selected: offered=["run-with-fixed-deadline"]; selected=run-with-fixed-deadline; reason=caller-bound;budget-seconds=10; result=awaiting-process
- ground [1789025983223] applied: offered=["run-with-fixed-deadline"]; selected=run-with-fixed-deadline; reason=deadline-and-output-bound-enforced; result=exit=0
- freshness [1789025983831] selected: offered=["run-with-fixed-deadline"]; selected=run-with-fixed-deadline; reason=caller-bound;budget-seconds=10; result=awaiting-process
- freshness [1789025983831] applied: offered=["run-with-fixed-deadline"]; selected=run-with-fixed-deadline; reason=deadline-and-output-bound-enforced; result=exit=0
- baseline-preflight [1789025984172] selected: offered=["run-with-fixed-deadline"]; selected=run-with-fixed-deadline; reason=caller-bound;budget-seconds=10; result=awaiting-process
- baseline-preflight [1789025984172] applied: offered=["run-with-fixed-deadline"]; selected=run-with-fixed-deadline; reason=deadline-and-output-bound-enforced; result=exit=0
- baseline [1789025984560] selected: offered=["run-with-fixed-deadline"]; selected=run-with-fixed-deadline; reason=caller-bound;budget-seconds=10; result=awaiting-process
- baseline [1789025984560] applied: offered=["run-with-fixed-deadline"]; selected=run-with-fixed-deadline; reason=deadline-and-output-bound-enforced; result=exit=0
- inventory [1789025984938] selected: offered=["run-with-fixed-deadline"]; selected=run-with-fixed-deadline; reason=caller-bound;budget-seconds=30; result=awaiting-process
- inventory [1789025984938] applied: offered=["run-with-fixed-deadline"]; selected=run-with-fixed-deadline; reason=deadline-and-output-bound-enforced; result=exit=0
- eval-model [1789025988648] selected: offered=["qwen38-q4"]; selected=qwen38-q4; reason=explicit-single-model-evaluation;other-models-and-remote-excluded; result=awaiting-generation
- eval-model [1789025988650] selected: offered=["run-with-dynamic-observation"]; selected=run-with-dynamic-observation; reason=progress-driven;explicit-stop-control;no-lifetime-deadline; result=awaiting-process
- eval-model [1789025989769] selected: offered=["continue","inspect","stop"]; selected=continue; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789025989769] applied: offered=["continue","inspect","stop"]; selected=continue; reason=process-state-is-not-semantic-progress; result=progress-observation-continues
- eval-model [1789025992806] selected: offered=["continue","inspect","stop"]; selected=inspect; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789025992806] applied: offered=["continue","inspect","stop"]; selected=inspect; reason=process-state-is-not-semantic-progress; result=15513 R      0:02.85 3243296
- eval-model [1789026000932] selected: offered=["continue","inspect","stop"]; selected=inspect; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789026000932] applied: offered=["continue","inspect","stop"]; selected=inspect; reason=process-state-is-not-semantic-progress; result=15513 R      0:09.86 13130816
- eval-model [1789026025297] selected: offered=["continue","inspect","stop"]; selected=inspect; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789026025297] applied: offered=["continue","inspect","stop"]; selected=inspect; reason=process-state-is-not-semantic-progress; result=15513 R      0:32.50 956576
- eval-model [1789026055323] selected: offered=["continue","inspect","stop"]; selected=inspect; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789026055323] applied: offered=["continue","inspect","stop"]; selected=inspect; reason=process-state-is-not-semantic-progress; result=15513 S      0:38.61 856160
- eval-model [1789026085387] selected: offered=["continue","inspect","stop"]; selected=inspect; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789026085387] applied: offered=["continue","inspect","stop"]; selected=inspect; reason=process-state-is-not-semantic-progress; result=15513 S      0:39.95 859360
- eval-model [1789026115458] selected: offered=["continue","inspect","stop"]; selected=inspect; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789026115458] applied: offered=["continue","inspect","stop"]; selected=inspect; reason=process-state-is-not-semantic-progress; result=15513 S      0:41.32 887792
- eval-model [1789026145503] selected: offered=["continue","inspect","stop"]; selected=inspect; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789026145503] applied: offered=["continue","inspect","stop"]; selected=inspect; reason=process-state-is-not-semantic-progress; result=15513 S      0:42.62 890960
- eval-model [1789026175504] selected: offered=["continue","inspect","stop"]; selected=continue; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789026175504] applied: offered=["continue","inspect","stop"]; selected=continue; reason=process-state-is-not-semantic-progress; result=progress-observation-continues
- eval-model [1789026205541] selected: offered=["continue","inspect","stop"]; selected=continue; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789026205541] applied: offered=["continue","inspect","stop"]; selected=continue; reason=process-state-is-not-semantic-progress; result=progress-observation-continues
- eval-model [1789026235553] selected: offered=["continue","inspect","stop"]; selected=continue; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789026235553] applied: offered=["continue","inspect","stop"]; selected=continue; reason=process-state-is-not-semantic-progress; result=progress-observation-continues
- eval-model [1789026265589] selected: offered=["continue","inspect","stop"]; selected=continue; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789026265589] applied: offered=["continue","inspect","stop"]; selected=continue; reason=process-state-is-not-semantic-progress; result=progress-observation-continues
- eval-model [1789026295609] selected: offered=["continue","inspect","stop"]; selected=continue; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789026295609] applied: offered=["continue","inspect","stop"]; selected=continue; reason=process-state-is-not-semantic-progress; result=progress-observation-continues
- eval-model [1789026325649] selected: offered=["continue","inspect","stop"]; selected=continue; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789026325649] applied: offered=["continue","inspect","stop"]; selected=continue; reason=process-state-is-not-semantic-progress; result=progress-observation-continues
- eval-model [1789026355695] selected: offered=["continue","inspect","stop"]; selected=continue; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789026355695] applied: offered=["continue","inspect","stop"]; selected=continue; reason=process-state-is-not-semantic-progress; result=progress-observation-continues
- eval-model [1789026385703] selected: offered=["continue","inspect","stop"]; selected=continue; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789026385703] applied: offered=["continue","inspect","stop"]; selected=continue; reason=process-state-is-not-semantic-progress; result=progress-observation-continues
- eval-model [1789026415744] selected: offered=["continue","inspect","stop"]; selected=continue; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789026415744] applied: offered=["continue","inspect","stop"]; selected=continue; reason=process-state-is-not-semantic-progress; result=progress-observation-continues
- eval-model [1789026445745] selected: offered=["continue","inspect","stop"]; selected=continue; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789026445745] applied: offered=["continue","inspect","stop"]; selected=continue; reason=process-state-is-not-semantic-progress; result=progress-observation-continues
- eval-model [1789026475761] selected: offered=["continue","inspect","stop"]; selected=continue; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789026475761] applied: offered=["continue","inspect","stop"]; selected=continue; reason=process-state-is-not-semantic-progress; result=progress-observation-continues
- eval-model [1789026505802] selected: offered=["continue","inspect","stop"]; selected=continue; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789026505802] applied: offered=["continue","inspect","stop"]; selected=continue; reason=process-state-is-not-semantic-progress; result=progress-observation-continues
- eval-model [1789026535804] selected: offered=["continue","inspect","stop"]; selected=continue; reason=observed-progress-gap; result=awaiting-action
- eval-model [1789026535804] applied: offered=["continue","inspect","stop"]; selected=continue; reason=process-state-is-not-semantic-progress; result=progress-observation-continues
- eval-model [1789025988650] applied: offered=["run-with-dynamic-observation"]; selected=run-with-dynamic-observation; reason=dynamic-observation-returned; result=exit=0
- eval-model [1789025988648] applied: offered=["qwen38-q4"]; selected=qwen38-q4; reason=explicit-single-model-evaluation;other-models-and-remote-excluded; result=generation-exit=0
- check-eval-model-preflight [1789026570478] selected: offered=["run-with-fixed-deadline"]; selected=run-with-fixed-deadline; reason=caller-bound;budget-seconds=10; result=awaiting-process
- check-eval-model-preflight [1789026570478] applied: offered=["run-with-fixed-deadline"]; selected=run-with-fixed-deadline; reason=deadline-and-output-bound-enforced; result=exit=0
- check-eval-model [1789026571333] selected: offered=["run-with-fixed-deadline"]; selected=run-with-fixed-deadline; reason=caller-bound;budget-seconds=10; result=awaiting-process
- check-eval-model [1789026571333] applied: offered=["run-with-fixed-deadline"]; selected=run-with-fixed-deadline; reason=deadline-and-output-bound-enforced; result=exit=0
- evaluation-outcome [1789026628006] selected: offered=["finish-evaluation:repaired"]; selected=finish-evaluation:repaired; reason=single-case-contract;no-retry-or-budget-extension; result=awaiting-evidence-and-release
- evaluation-outcome [1789026628006] applied: offered=["finish-evaluation:repaired"]; selected=finish-evaluation:repaired; reason=result-saved;snapshot-released; result=source-unchanged=0

## Learning rounds

[{"route":"eval-model","state":"evaluation-excluded","example":"evaluation-excluded","worker":"excluded","status":".hearth/session-learning"}]
