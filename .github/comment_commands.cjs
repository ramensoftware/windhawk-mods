// @ts-check
// Helpers shared by the scripts that run commands from issue and pull request
// comments.

/** @typedef {import('@actions/github-script').AsyncFunctionArguments} ScriptArguments */
/** @typedef {ScriptArguments['github']} Github */
/** @typedef {ScriptArguments['context']} Context */
/** @typedef {ScriptArguments['core']} Core */

/** @typedef {NonNullable<Parameters<Github['rest']['reactions']['createForIssueComment']>[0]>['content']} ReactionContent */

/**
 * Adds a reaction to a comment. A reaction is a best-effort acknowledgement, so
 * a comment that was deleted in the meantime is logged and ignored instead of
 * failing the command it acknowledges.
 *
 * @param {object} params
 * @param {Github} params.github
 * @param {Core} params.core
 * @param {string} params.owner
 * @param {string} params.repo
 * @param {number} params.commentId
 * @param {ReactionContent} params.content
 */
async function react({ github, core, owner, repo, commentId, content }) {
  try {
    await github.rest.reactions.createForIssueComment({
      owner,
      repo,
      comment_id: commentId,
      content,
    });
  } catch (error) {
    if (/** @type {{ status?: number }} */ (error).status !== 404) {
      throw error;
    }

    core.info(`Skipped the ${content} reaction, comment ${commentId} is gone`);
  }
}

/**
 * Runs the function, and if it throws, comments the message with a link to the
 * workflow run, then rethrows so that the run fails and the error is visible.
 *
 * @template T
 * @param {object} params
 * @param {Github} params.github
 * @param {Context} params.context
 * @param {Core} params.core
 * @param {number} params.issueNumber The issue or pull request to comment on.
 * @param {string} params.message
 * @param {() => Promise<T>} run
 * @returns {Promise<T>}
 */
async function commentOnFailure({ github, context, core, issueNumber, message }, run) {
  try {
    return await run();
  } catch (error) {
    const { owner, repo } = context.repo;
    try {
      await github.rest.issues.createComment({
        owner,
        repo,
        issue_number: issueNumber,
        body:
          `${message} ` +
          `See [the workflow run](${context.serverUrl}/${owner}/${repo}/actions/runs/${context.runId}) for details.`,
      });
    } catch (reportError) {
      core.info(`Failed to report the error: ${reportError}`);
    }

    throw error;
  }
}

module.exports = {
  react,
  commentOnFailure,
};
