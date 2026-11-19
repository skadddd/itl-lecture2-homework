#!/usr/bin/env bash
# 手动创建 GitHub 仓库并推送当前 Git 仓库。
# 用法： ./push_to_github.sh [仓库名]
# 默认仓库名： itl-lecture2-homework
#
# 安全说明：
#   - Token 通过 read -rs 从终端读取，不回显、不写入任何文件、不进入 git 配置。
#   - 仅保存在当前进程内存中，脚本结束后即释放。
set -euo pipefail

REPO_NAME="${1:-itl-lecture2-homework}"
OWNER="skadddd"

read -rs -p "GitHub Personal Access Token (classic 需勾选 repo): " TOKEN
printf '\n'

echo "[1/3] 创建仓库 https://github.com/${OWNER}/${REPO_NAME} ..."
CREATE_RESULT=$(curl -sS -X POST https://api.github.com/user/repos \
  -H "Authorization: Bearer ${TOKEN}" \
  -H "Accept: application/vnd.github+json" \
  -d "{\"name\":\"${REPO_NAME}\",\"private\":false,\"description\":\"ITL lecture 2 homework: C++/CMake and ROS 2 nodes\"}")

if printf '%s' "$CREATE_RESULT" | grep -q '"full_name"'; then
  echo "  仓库就绪。"
else
  echo "  创建失败，GitHub 响应："
  echo "$CREATE_RESULT"
  unset TOKEN
  exit 1
fi

echo "[2/3] 设置 remote ..."
git remote remove origin 2>/dev/null || true
git remote add origin "https://github.com/${OWNER}/${REPO_NAME}.git"

echo "[3/3] 推送提交 ..."
git -c "http.extraheader=Authorization: Bearer ${TOKEN}" push -u origin HEAD

unset TOKEN
echo "完成： https://github.com/${OWNER}/${REPO_NAME}"
