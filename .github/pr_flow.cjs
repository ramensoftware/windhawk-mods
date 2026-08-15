// @ts-check
// Label state machine and comment commands for the pull request review
// process, documented at:
// https://github.com/ramensoftware/windhawk/wiki/Mod-submission-review-process
//
// Loaded by actions/github-script from the pr_labels and pr_commands workflows.
// All API calls run as the windhawk-reviewer app.

/** @typedef {import('@actions/github-script').AsyncFunctionArguments} ScriptArguments */
/** @typedef {ScriptArguments['github']} Github */
/** @typedef {ScriptArguments['context']} Context */
/** @typedef {ScriptArguments['core']} Core */

// context.payload is loosely typed, matching the action, so each handler
// narrows it to the event it handles.
/** @typedef {import('@octokit/webhooks-types').PullRequestEvent} PullRequestEvent */
/** @typedef {import('@octokit/webhooks-types').IssueCommentCreatedEvent | import('@octokit/webhooks-types').IssueCommentEditedEvent} IssueCommentEvent */
/** @typedef {import('@octokit/webhooks-types').AuthorAssociation} AuthorAssociation */

/** @typedef {Awaited<ReturnType<Github['rest']['pulls']['get']>>['data']} RestPullRequest */
/** @typedef {Awaited<ReturnType<Github['rest']['issues']['listComments']>>['data'][number]} RestComment */
/** @typedef {NonNullable<Parameters<Github['rest']['reactions']['createForIssueComment']>[0]>['content']} ReactionContent */

const WAITING_FOR_AUTHOR = 'waiting-for-author';
const WAITING_FOR_AI_REVIEW = 'waiting-for-ai-review';
const WAITING_FOR_REVIEWER = 'waiting-for-reviewer';

// Every open pull request carries exactly one of these.
const FLOW_LABELS = [
  WAITING_FOR_AUTHOR,
  WAITING_FOR_AI_REVIEW,
  WAITING_FOR_REVIEWER,
];

const WIKI_URL =
  'https://github.com/ramensoftware/windhawk/wiki/Mod-submission-review-process';

const AI_REVIEW_COMMAND = '/ai-review';
const READY_FOR_REVIEWER_COMMAND = '/ready-for-reviewer';

// The AI reviewer stamps the reviewed commit into a hidden marker in its review
// comment. It's the only record of which commit a review covers, and it's what
// /ready-for-reviewer checks against the current head commit.
const AI_REVIEW_MARKER_PATTERN = /<!--\s*ai-review\s+sha=([0-9a-f]{40})\s*-->/g;

// Comments from these association levels are trusted to drive the flow. The
// association comes from the event payload, which avoids an extra API call and
// an extra app permission, and is only an approximation of write access: an
// organization member, or a collaborator invited with read or triage access, is
// included as well.
/** @type {AuthorAssociation[]} */
const PRIVILEGED_ASSOCIATIONS = ['OWNER', 'MEMBER', 'COLLABORATOR'];

// An empty list applies the flow to every pull request. Fill it with pull
// request numbers to limit the flow to those while trying something out.
/** @type {number[]} */
const ENABLED_PR_NUMBERS = [];

/**
 * @param {number} prNumber
 * @returns {boolean}
 */
function isFlowEnabled(prNumber) {
  return ENABLED_PR_NUMBERS.length === 0 || ENABLED_PR_NUMBERS.includes(prNumber);
}

// The flow doesn't start until the author asks for a review, so a newly opened
// pull request gets the instructions it takes to move it forward.
const WELCOME_COMMENT_BODY =
  'Thanks for the pull request! This repository uses a two-stage review: an AI ' +
  'review that you run yourself, followed by a human review.\n\n' +
  `To get started, comment \`${AI_REVIEW_COMMAND}\`. Once you're happy with the ` +
  `result, comment \`${READY_FOR_REVIEWER_COMMAND}\` to hand it over to a human ` +
  'reviewer.\n\n' +
  `See the [pull request review process](${WIKI_URL}) for the full details.`;

/**
 * @param {{ name: string }[]} labels
 * @returns {string[]}
 */
function flowLabelsOf(labels) {
  return labels
    .map((label) => label.name)
    .filter((name) => FLOW_LABELS.includes(name));
}

/**
 * Reads the flow labels that are on the pull request right now. An event payload
 * carries the labels from the moment the event fired, which another run of the
 * flow may have changed by the time this one starts.
 *
 * @param {object} params
 * @param {Github} params.github
 * @param {string} params.owner
 * @param {string} params.repo
 * @param {number} params.prNumber
 * @returns {Promise<string[]>}
 */
async function getCurrentFlowLabels({ github, owner, repo, prNumber }) {
  const labels = await github.paginate(github.rest.issues.listLabelsOnIssue, {
    owner,
    repo,
    issue_number: prNumber,
    per_page: 100,
  });

  return flowLabelsOf(labels);
}

/**
 * @param {object} params
 * @param {Github} params.github
 * @param {string} params.owner
 * @param {string} params.repo
 * @param {number} params.prNumber
 * @param {string[]} params.labels
 */
async function removeLabels({ github, owner, repo, prNumber, labels }) {
  for (const label of labels) {
    try {
      await github.rest.issues.removeLabel({
        owner,
        repo,
        issue_number: prNumber,
        name: label,
      });
    } catch (error) {
      // A concurrent run may have removed it first.
      if (/** @type {{ status?: number }} */ (error).status !== 404) {
        throw error;
      }
    }
  }
}

/**
 * Takes the pull request out of the flow entirely, for when it's no longer open.
 *
 * @param {object} params
 * @param {Github} params.github
 * @param {Core} params.core
 * @param {string} params.owner
 * @param {string} params.repo
 * @param {number} params.prNumber
 * @param {string[]} params.currentFlowLabels
 */
async function removeFlowLabels({ github, core, owner, repo, prNumber, currentFlowLabels }) {
  if (currentFlowLabels.length === 0) {
    return;
  }

  await removeLabels({ github, owner, repo, prNumber, labels: currentFlowLabels });
  core.info(`Removed ${currentFlowLabels.join(', ')} from pull request #${prNumber}`);
}

/**
 * Moves the pull request to `label`, removing whichever other flow labels are
 * present. The new label is added before the old ones are removed, so the pull
 * request is never left without one.
 *
 * @param {object} params
 * @param {Github} params.github
 * @param {Core} params.core
 * @param {string} params.owner
 * @param {string} params.repo
 * @param {number} params.prNumber
 * @param {string[]} params.currentFlowLabels
 * @param {string} params.label
 * @returns {Promise<boolean>} Whether anything changed.
 */
async function setFlowLabel({ github, core, owner, repo, prNumber, currentFlowLabels, label }) {
  if (currentFlowLabels.length === 1 && currentFlowLabels[0] === label) {
    core.info(`Pull request #${prNumber} is already labeled ${label}`);
    return false;
  }

  if (!currentFlowLabels.includes(label)) {
    await github.rest.issues.addLabels({
      owner,
      repo,
      issue_number: prNumber,
      labels: [label],
    });
  }

  await removeLabels({
    github,
    owner,
    repo,
    prNumber,
    labels: currentFlowLabels.filter((name) => name !== label),
  });

  core.info(`Labeled pull request #${prNumber} as ${label}`);
  return true;
}

/**
 * @param {object} params
 * @param {Github} params.github
 * @param {string} params.owner
 * @param {string} params.repo
 * @param {number} params.prNumber
 * @param {string} params.body
 */
function postComment({ github, owner, repo, prNumber, body }) {
  return github.rest.issues.createComment({
    owner,
    repo,
    issue_number: prNumber,
    body,
  });
}

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
 * The AI review only handles a pull request that adds or updates a single mod
 * file, which is also what the rest of the automated checks expect.
 *
 * @param {object} params
 * @param {Github} params.github
 * @param {string} params.owner
 * @param {string} params.repo
 * @param {number} params.prNumber
 * @returns {Promise<string | null>} The reason the pull request is out of
 *   scope, or null when it's in scope.
 */
async function getOutOfScopeReason({ github, owner, repo, prNumber }) {
  const { data: files } = await github.rest.pulls.listFiles({
    owner,
    repo,
    pull_number: prNumber,
    per_page: 100,
  });

  if (files.length === 0) {
    return "it doesn't change any files";
  }

  if (files.length > 1) {
    const count = files.length === 100 ? '100 or more' : `${files.length}`;
    return `it changes ${count} files`;
  }

  const file = files[0];

  if (!file.filename.startsWith('mods/') || !file.filename.endsWith('.wh.cpp')) {
    return `\`${file.filename}\` isn't a mod file`;
  }

  if (file.status !== 'added' && file.status !== 'modified') {
    return `\`${file.filename}\` is marked as ${file.status}, and only an added or a modified mod file can be reviewed`;
  }

  return null;
}

/**
 * @param {object} params
 * @param {Github} params.github
 * @param {string} params.owner
 * @param {string} params.repo
 * @param {number} params.prNumber
 * @param {string} params.botLogin
 * @returns {Promise<string | null>} The commit covered by the most recent AI
 *   review, or null if there isn't one.
 */
async function getLastReviewedSha({ github, owner, repo, prNumber, botLogin }) {
  /** @type {RestComment[]} */
  const comments = await github.paginate(github.rest.issues.listComments, {
    owner,
    repo,
    issue_number: prNumber,
    per_page: 100,
  });

  /** @type {string | null} */
  let sha = null;

  // Comments come back oldest first, so the last marker found is the newest one.
  for (const comment of comments) {
    if (comment.user?.login !== botLogin) {
      continue;
    }

    for (const match of (comment.body ?? '').matchAll(AI_REVIEW_MARKER_PATTERN)) {
      sha = match[1];
    }
  }

  return sha;
}

/**
 * A command is a line of its own, either the first or the last one, so that it
 * can lead a comment or follow the explanation it goes with. The first line wins
 * when both are commands.
 *
 * @param {string} body
 * @returns {typeof AI_REVIEW_COMMAND | typeof READY_FOR_REVIEWER_COMMAND | null}
 */
function parseCommand(body) {
  const lines = body
    .split('\n')
    .map((line) => line.trim())
    .filter((line) => line !== '');

  for (const line of [lines[0], lines[lines.length - 1]]) {
    if (line === AI_REVIEW_COMMAND || line === READY_FOR_REVIEWER_COMMAND) {
      return line;
    }
  }

  return null;
}

/**
 * Handles pull_request_target. Keeps a flow label on the pull request, and
 * takes a pull request that got new commits out of the human review queue.
 *
 * @param {object} params
 * @param {Github} params.github
 * @param {Context} params.context
 * @param {Core} params.core
 */
async function handlePullRequestEvent({ github, context, core }) {
  const { owner, repo } = context.repo;
  const payload = /** @type {PullRequestEvent} */ (context.payload);
  const pullRequest = payload.pull_request;
  const prNumber = pullRequest.number;

  if (!isFlowEnabled(prNumber)) {
    core.info(`The flow is not enabled for pull request #${prNumber}`);
    return;
  }

  const currentFlowLabels = await getCurrentFlowLabels({ github, owner, repo, prNumber });

  const args = { github, core, owner, repo, prNumber, currentFlowLabels };

  // A pull request that is no longer open is out of the flow, and the labels
  // only describe whose turn it is while it's open.
  if (payload.action === 'closed') {
    await removeFlowLabels(args);
    return;
  }

  // New commits, or a conversion back to a draft, take the pull request out of
  // the human review queue.
  if (
    (payload.action === 'synchronize' || payload.action === 'converted_to_draft') &&
    currentFlowLabels.includes(WAITING_FOR_REVIEWER)
  ) {
    await setFlowLabel({ ...args, label: WAITING_FOR_AUTHOR });
    await postComment({
      github,
      owner,
      repo,
      prNumber,
      body:
        payload.action === 'synchronize'
          ? `New commits were pushed, so this pull request left the human review queue and is back to **${WAITING_FOR_AUTHOR}**.\n\n` +
            `Comment \`${AI_REVIEW_COMMAND}\` to get an AI review of the updated code, then \`${READY_FOR_REVIEWER_COMMAND}\` to hand it over to a human reviewer again. ` +
            `See the [pull request review process](${WIKI_URL}) for details.`
          : `This pull request was converted to a draft, so it left the human review queue and is back to **${WAITING_FOR_AUTHOR}**.\n\n` +
            `Mark it as ready for review and comment \`${READY_FOR_REVIEWER_COMMAND}\` to hand it over to a human reviewer again. ` +
            `See the [pull request review process](${WIKI_URL}) for details.`,
    });
    return;
  }

  // Covers a newly opened pull request, and repairs a pull request that lost its
  // label or ended up with more than one.
  if (currentFlowLabels.length !== 1) {
    await setFlowLabel({ ...args, label: WAITING_FOR_AUTHOR });
  }

  // Only when opened, so that reopening a pull request doesn't repeat it.
  if (payload.action === 'opened') {
    await postComment({ github, owner, repo, prNumber, body: WELCOME_COMMENT_BODY });
  }
}

/**
 * @param {object} params
 * @param {Github} params.github
 * @param {Core} params.core
 * @param {string} params.owner
 * @param {string} params.repo
 * @param {RestPullRequest} params.pullRequest
 * @param {string[]} params.currentFlowLabels
 * @returns {Promise<string | null>} The reason the command was refused, or null
 *   when it was carried out.
 */
async function runAiReview({ github, core, owner, repo, pullRequest, currentFlowLabels }) {
  const prNumber = pullRequest.number;

  if (pullRequest.state !== 'open') {
    return 'this pull request is closed.';
  }

  if (currentFlowLabels.includes(WAITING_FOR_AI_REVIEW)) {
    return 'an AI review was already requested, please wait for it to be posted.';
  }

  const outOfScopeReason = await getOutOfScopeReason({ github, owner, repo, prNumber });
  if (outOfScopeReason) {
    return (
      `the AI review only handles a pull request that adds or updates a single mod file under \`mods/\`, but ${outOfScopeReason}.\n\n` +
      `Comment \`${READY_FOR_REVIEWER_COMMAND}\` to hand this pull request over to a human reviewer directly. ` +
      `See the [pull request review process](${WIKI_URL}) for details.`
    );
  }

  await setFlowLabel({
    github,
    core,
    owner,
    repo,
    prNumber,
    currentFlowLabels,
    label: WAITING_FOR_AI_REVIEW,
  });
  return null;
}

/**
 * @param {object} params
 * @param {Github} params.github
 * @param {Core} params.core
 * @param {string} params.owner
 * @param {string} params.repo
 * @param {RestPullRequest} params.pullRequest
 * @param {string[]} params.currentFlowLabels
 * @param {string} params.botLogin
 * @returns {Promise<string | null>} The reason the command was refused, or null
 *   when it was carried out.
 */
async function runReadyForReviewer({
  github,
  core,
  owner,
  repo,
  pullRequest,
  currentFlowLabels,
  botLogin,
}) {
  const prNumber = pullRequest.number;

  if (pullRequest.state !== 'open') {
    return 'this pull request is closed.';
  }

  if (pullRequest.draft) {
    return 'this pull request is a draft. Mark it as ready for review first.';
  }

  // Only the settled state is refused, so that repeating the command repairs a
  // pull request that was left with an extra flow label.
  if (currentFlowLabels.length === 1 && currentFlowLabels[0] === WAITING_FOR_REVIEWER) {
    return 'this pull request is already waiting for a human reviewer.';
  }

  if (currentFlowLabels.includes(WAITING_FOR_AI_REVIEW)) {
    return 'an AI review is being prepared, please wait for it to be posted.';
  }

  // A pull request the AI reviewer can't handle goes straight to a human, so
  // there's nothing to check it against.
  const outOfScopeReason = await getOutOfScopeReason({ github, owner, repo, prNumber });
  if (!outOfScopeReason) {
    const headSha = pullRequest.head.sha;
    const reviewedSha = await getLastReviewedSha({ github, owner, repo, prNumber, botLogin });

    if (!reviewedSha) {
      return (
        `this pull request hasn't been through an AI review yet. Comment \`${AI_REVIEW_COMMAND}\` first.\n\n` +
        `See the [pull request review process](${WIKI_URL}) for details.`
      );
    }

    if (reviewedSha !== headSha) {
      return (
        `the most recent AI review covers ${reviewedSha.slice(0, 7)}, but the current head of this pull request is ${headSha.slice(0, 7)}. ` +
        `Comment \`${AI_REVIEW_COMMAND}\` to get a review of the current code.`
      );
    }
  }

  await setFlowLabel({
    github,
    core,
    owner,
    repo,
    prNumber,
    currentFlowLabels,
    label: WAITING_FOR_REVIEWER,
  });
  return null;
}

/**
 * Handles issue_comment. Runs the flow commands on behalf of the pull request
 * author and of any trusted commenter, and ignores everyone else.
 *
 * @param {object} params
 * @param {Github} params.github
 * @param {Context} params.context
 * @param {Core} params.core
 * @param {string} params.botLogin
 */
async function handleComment({ github, context, core, botLogin }) {
  const { owner, repo } = context.repo;
  const payload = /** @type {IssueCommentEvent} */ (context.payload);
  const comment = payload.comment;
  const issue = payload.issue;

  if (!issue.pull_request) {
    return;
  }

  if (!isFlowEnabled(issue.number)) {
    core.info(`The flow is not enabled for pull request #${issue.number}`);
    return;
  }

  if (comment.user.type === 'Bot') {
    return;
  }

  // A closed pull request is out of the flow, and its labels are removed.
  if (issue.state === 'closed') {
    core.info(`Ignoring a comment on closed pull request #${issue.number}`);
    return;
  }

  try {
    await runComment({ github, core, owner, repo, payload, comment, issue, botLogin });
  } catch (error) {
    // Tell the commenter, then let the run fail so that the error is visible.
    try {
      await postComment({
        github,
        owner,
        repo,
        prNumber: issue.number,
        body:
          `Something went wrong while handling this comment. ` +
          `See [the workflow run](${context.serverUrl}/${owner}/${repo}/actions/runs/${context.runId}) for details.`,
      });
    } catch (reportError) {
      core.info(`Failed to report the error: ${reportError}`);
    }

    throw error;
  }
}

/**
 * @param {object} params
 * @param {Github} params.github
 * @param {Core} params.core
 * @param {string} params.owner
 * @param {string} params.repo
 * @param {IssueCommentEvent} params.payload
 * @param {IssueCommentEvent['comment']} params.comment
 * @param {IssueCommentEvent['issue']} params.issue
 * @param {string} params.botLogin
 */
async function runComment({ github, core, owner, repo, payload, comment, issue, botLogin }) {
  const commenterLogin = comment.user.login;

  const command = parseCommand(comment.body);

  if (!command) {
    core.info('The comment is not a flow command');
    return;
  }

  // An edit acts only on a command that the comment didn't carry before, which
  // is what makes it possible to add one to a comment after posting it. A
  // command that was already there was acted on when the comment was posted, and
  // reworking the prose around it doesn't ask for it again.
  if (payload.action === 'edited') {
    const previousBody = payload.changes.body?.from;

    if (previousBody === undefined) {
      core.info(`Ignoring an edit to comment ${comment.id}, which left the body as it was`);
      return;
    }

    if (parseCommand(previousBody) === command) {
      core.info(`Ignoring an edit to comment ${comment.id}, which was already acted on`);
      return;
    }
  }

  const isAuthorized =
    PRIVILEGED_ASSOCIATIONS.includes(comment.author_association) ||
    commenterLogin === issue.user.login;

  if (!isAuthorized) {
    core.info(`Ignoring ${command} from ${commenterLogin}`);
    await react({ github, core, owner, repo, commentId: comment.id, content: '-1' });
    return;
  }

  const { data: pullRequest } = await github.rest.pulls.get({
    owner,
    repo,
    pull_number: issue.number,
  });
  const currentFlowLabels = flowLabelsOf(pullRequest.labels ?? []);
  const args = { github, core, owner, repo, pullRequest, currentFlowLabels };

  const refusalReason =
    command === AI_REVIEW_COMMAND
      ? await runAiReview(args)
      : await runReadyForReviewer({ ...args, botLogin });

  if (refusalReason) {
    core.info(`Refused ${command}: ${refusalReason}`);
    await react({ github, core, owner, repo, commentId: comment.id, content: 'confused' });
    await postComment({
      github,
      owner,
      repo,
      prNumber: issue.number,
      body: `@${commenterLogin} \`${command}\` can't be applied here: ${refusalReason}`,
    });
    return;
  }

  await react({ github, core, owner, repo, commentId: comment.id, content: 'eyes' });
}

module.exports = {
  handlePullRequestEvent,
  handleComment,
};
