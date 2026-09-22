# Native answer length

The source-backed form-cli accepts:

```text
generate --words 350:450 --tokens 2048 <enquiry and source context>
generate --reasoning 2048 --words 350:450 --tokens 2048 <enquiry and source context>
```

Options may appear in any order before the prompt. Bounds are positive whole
numbers with minimum no greater than maximum. Repeated or malformed options
return before model admission. Prompt bytes after the options are preserved;
the admitted request also carries the explicit word-range instruction.

The runtime counts runs of non-whitespace bytes, using ASCII space, tab, newline,
vertical tab, form feed and carriage return as separators. Headings count.
This is a stated counting convention, not language-independent segmentation.

A completed answer outside the range receives one measured observation in the
same live model session. The model revises with the original context and its
actual answer still present. This continuation neither reloads the weights nor
replays the original prompt. Its output allowance is the selected `--tokens`
value. The runtime reserves room for this correction before admission and checks
the actual encoded observation again before submission.

Qwen observations use the selected model's tool-message layout: a `user` turn
containing `<tool_response>` markup, followed by the assistant opening. The
native GGUF reader exposes that contract in `tokenizer.chat_template`; tool
output remains identified inside the markup. Direct user questions keep their
own ordinary message path.

The returned metadata distinguishes model completion, word-range success,
retained evidence and resource release. A second out-of-range answer remains
failed; there is no automatic retry loop. An unfinished initial answer stays
unfinished and does not enter length correction. No text is clipped or padded
to obtain a passing count.

Private evidence retains the initial stages, original final answer, measured
feedback, correction and returned answer. `length_revision_generated_ids`,
`injected_ids`, `initial_words`, `final_words` and `length_revisions` expose the
additional work. Without `--words`, the existing generation choices remain.

Word-count success establishes that requirement alone. Grounding, complete
coverage, insight, warmth and usefulness still require reading the answer.
